typedef struct {
    float3 pos;//position of the collision
    float3 normal;//normal of the collision
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
    float c = 1.f;

    if(true){
        float light = 1e4;
        float3 lightPos = *pos;

        float3 E = normalize(*pos-ray.pos);
        float3 l = normalize(lightPos-ray.pos);
        float3 R = 2*dot(l,ray.normal)*ray.normal - l;

        float cosTheta = clamp(dot(l, ray.normal), 0.f, 1.f);
        float cosAlpha = clamp(dot(E, R), 0.f, .1f);
        float ambiant = 0.7f;
        c = ambiant +  light * (cosTheta + pow(cosAlpha, 10)) / (ray.distance*ray.distance);
    }

    //swap the bits in value to use it as a colour
    int swaping[] = {17,5,15,10,21,11,22,9,1,19,0,3,16,20,18,14,4,6,23,2,7,13,8,12};
    int val0 = ray.value, val = 0;
    for(int i = 0;i<24;i++){
        val |= ((1<<i)&val0)>>i<<swaping[i];
    }
    float3 colour = (float3)(val&255, (val>>8)&255, (val>>16)&255)/255;
    //if(colour.x < 0) colour = (float3)(1.f);

    float4 imgVal = (float4)(c*colour, 1.0f);
    write_imagef(image, id, imgVal);
}

inline DataRay raycast(float x, float y, float z, float dx, float dy, float dz,__global Node *nodes, int len, int topID){
    if(x>=0 && y>=0 && z>=0 && x<len && y<len && z<len){
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
        float3 nTab[6] = {{1,0,0},{0,1,0},{0,0,1}, {-1,0,0},{0,-1,0},{0,0,-1}};//normal tab
        float distance = 0;
        
        
        
        for(int i = node.level-1;node.value == -1;i--){
            id = node.subNodes[tz>>i&1][ty>>i&1][tx>>i&1];
            ids[i] = id;
            node = nodes[id];
        }
        int currentValue = node.value;
        while(node.value == currentValue){//for(int it = 0;it<10 && node.value == currentValue;it++){//
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
            
            if(x>=0 && y>=0 && z>=0 && x<len && y<len && z<len){
                node = nodes[topID];//TODO don't come back to the top each time, find the nearest node to go down again
                for(int i = node.level-1;node.value == -1;i--){
                    id = node.subNodes[tz>>i&1][ty>>i&1][tx>>i&1];
                    ids[i] = id;
                    node = nodes[id];
                }
            }else{//out of the tree
                node.value = -1;//set value to -1 to say out of the tree
                //break;//TODO find why when this break is not in comment the program doesn't behave properly
            }
        }
        
        //remove the last epsilon to come back on the face of the collided cube
        x -= epsilon*dx;
        y -= epsilon*dy;
        z -= epsilon*dz;

        data.pos = (float3)(x,y,z);
        data.normal = nTab[rayID];
        data.value = node.value;
        data.distance = distance;
        
        return data;
    }else{//out of the tree
        return (DataRay){{0,0,0},{0,0,0},-1,0};
    }
}


