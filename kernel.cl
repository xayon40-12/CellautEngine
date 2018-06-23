typedef struct {
    float x,y,z;//position of the collision
    float nx,ny,nz;//normal of the collision
    int value;
    float distance;//total distance from the origine of the ray to the collision
} DataRay;

typedef struct {
    int subNodes[2][2][2];//[z][y][x]
    //all subnodes MUST have the same level
    int value;
    int level;//a leaf is level 0, then each branch above is +1
} Node;

DataRay raycast(float x, float y, float z, float dx, float dy, float dz,__global Node *nodes, int len, int topID);

__kernel void render(__global Node *nodes, __constant int *len, __constant int *topID, __constant float3 *X,
                        __constant float3 *Y, __constant float3 *Z, __constant float3 *pos, __write_only image2d_t image){
    const int2 id = {get_global_id(0), get_global_id(1)};
    const int2 size = {get_global_size(0), get_global_size(1)};
    const float2 coord = {(float)id.x*2/size.y - (float)size.x/size.y, (float)id.y*2/size.y - 1};
    float3 dir = -coord.x*(*X) + coord.y*(*Y) + (*Z);

    DataRay ray = raycast((*pos).x, (*pos).y, (*pos).z, dir.x, dir.y, dir.z, nodes, *len, *topID);

    float dist = ray.distance;
    float c = dist<1?0.5:100000/(dist*dist);
    c=c>0.5?0.5:c;
    float4 imgVal = (float4)(c*(1+ray.nx), c*(1+ray.ny), c*(1+ray.nz), 1.0f);

    imgVal = (float4)(dist,dist,dist,1.0f);
    imgVal = (float4)(ray.x/(*len),ray.y/(*len),ray.z/(*len),1.0f);
    write_imagef(image, id, imgVal);
}

DataRay raycast(float x, float y, float z, float dx, float dy, float dz,__global Node *nodes, int len, int topID){
    if(x<0 || y<0 || z<0 || x>len || y>len || z>len){//out of the tree
        return DataRay();
    }

    //first normalize direction
    float norm = sqrt(dx*dx + dy*dy + dz*dz);
    dx /= norm;
    dy /= norm;
    dz /= norm;

    float epsilon = 0.001;
    //if a coordinate is on a face of a cube there will be problems while raycasting so add an epsilon
    if(x == (int)x) x += epsilon;
    if(y == (int)y) y += epsilon;
    if(z == (int)z) z += epsilon;

    int tx = x, ty = y, tz = z;//t means temporary
    Node node = nodes[topID];
    int id, depth = node.level;
    int ids[32];//store each ids while descending the tree to climb after set
    ids[depth] = topID;

    DataRay data;
    int rayID = 0;
    float nTab[6][3] = {{1,0,0},{0,1,0},{0,0,1}, {-1,0,0},{0,-1,0},{0,0,-1}};//normal tab
    float distance = 0;



    for(int i = node.level-1;node.value == -1;i--){
        id = node.subNodes[tz>>i&1][ty>>i&1][tx>>i&1];
        ids[i] = id;
        node = nodes[id];
    }
    int currentValue = node.value;
    while(node.value == currentValue){
        int l = node.level, w = 1<<l;//d is the width of the cube
        int mask = 0b11111111111111111111111111111111>>l<<l;//use mask to clear all bits under level to have the coordinates
        // of the origine of the current subcube
        float ox = tx&mask, oy = ty&mask, oz = tz&mask;

        //with the normal "n", the cube origine "o", the ray position "p", the ray direction "d" and the variable "t":
        // the collision equation with a plan (one of the 6 in the cube) is: (o - (p + t*d))*n = 0
        // therefor to find the solution of "t" : t = ((o-p)*n)/(d*n)
        // here each normal are just one coordinate therefore the calculus becomes much simpler

        const float tTab[6] = {dx==0?-1:(ox-x)/dx,dy==0?-1:(oy-y)/dy,dz==0?-1:(oz-z)/dz,
                        dx==0?-1:(ox+w-x)/dx,dy==0?-1:(oy+w-y)/dy,dz==0?-1:(oz+w-z)/dz};//add w to have the opposite corner of the origine
        //the order of components in tTab is the same as in nTab

        rayID = 0;
        for(int i = 1;i<6;i++){
            rayID = tTab[rayID]<=0?i:(tTab[i]<tTab[rayID] && tTab[i]>0?i:rayID);//keep the smallest positive value
        }

        float t = tTab[rayID]+epsilon;//add an epsilon to be inside the collided cube for next loop
        x += t*dx;
        y += t*dy;
        z += t*dz;
        distance += t;// in fact t*norm but we normalized the direction at the beggining so norm = 1

        tx = x;
        ty = y;
        tz = z;

        if(x<0 || y<0 || z<0 || x>len || y>len || z>len){//out of the tree
            node.value = -1;//set value to -1 to say out of the tree
            break;
        }

        node = nodes[topID];//TODO don't come back to the top each time, find the nearest node to go down again
        for(int i = node.level-1;node.value == -1;i--){
            id = node.subNodes[tz>>i&1][ty>>i&1][tx>>i&1];
            ids[i] = id;
            node = nodes[id];
        }
    }

    //remove the last epsilon to come back on the face of the collided cube
    x -= epsilon*dx;
    y -= epsilon*dy;
    z -= epsilon*dz;

    data.x = x;
    data.y = y;
    data.z = z;
    float *n = nTab[rayID];
    data.nx = n[0];
    data.ny = n[1];
    data.nz = n[2];
    data.value = node.value;
    data.distance = distance;

    return data;
}
