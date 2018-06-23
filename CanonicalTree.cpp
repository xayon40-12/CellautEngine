//
// Created by Nathan Touroux on 29/05/2018.
//

#include <iostream>
#include "CanonicalTree.hpp"

std::unordered_map<Node, int> CanonicalTree::existingNodes;
std::vector<Node> CanonicalTree::nodes;
std::unordered_map<std::pair<int,int>, int> CanonicalTree::generatedTrees;

CanonicalTree::CanonicalTree(int level, int value): level(level), length(1<<level), width(length), px(0), py(0), pz(0), ratio(length/width){
    topID = generate(level, value);
}
CanonicalTree::CanonicalTree(int level, int value, float width): level(level), length(1<<level), width(width), px(0), py(0), pz(0), ratio((1<<level)/width){
    topID = generate(level, value);
}
CanonicalTree::CanonicalTree(int level, int value, float width, float px, float py, float pz): level(level), length(1<<level),
                                                                                               width(width),
                                                                                               px(px), py(py), pz(pz),
                                                                                               ratio(length/width){
    topID = generate(level, value);
}

CanonicalTree::CanonicalTree(const CanonicalTree &tree): level(tree.level), length(tree.length), topID(tree.topID), width(tree.width),
                                                         px(tree.px), py(tree.py), pz(tree.pz), ratio(tree.ratio) {

}

CanonicalTree::~CanonicalTree() {
    //TODO: remove from hasmap if necessary
}

int CanonicalTree::addNode(Node const &n) {
    auto it = existingNodes.find(n);
    if(it != existingNodes.end()){
        return it->second;
    }else{
        int id = nodes.size();//get the id of the next node to be added
        nodes.push_back(n);//add the node
        existingNodes[n] = id;//memoize it
        return id;
    }
}

Node CanonicalTree::addNodes(Node const &node){
    Node n;
    n.level = node.level+1;
    n.value = node.value;
    int id = addNode(node);

    for (int z = 0; z < 2; ++z) {
        for (int y = 0; y < 2; ++y) {
            for (int x = 0; x < 2; ++x) {
                n.subNodes[z][y][x] = id;
            }
        }
    }

    addNode(n);
    return n;
}

int CanonicalTree::generate(int level, int value) {
    int topID;
    std::pair<int,int> gen = {level, value};

    auto it = generatedTrees.find(gen);
    if(it != generatedTrees.end()){
        topID = it->second;
    }else{
        Node n(value);
        addNode(n);
        for (int i = 0; i < level; ++i) {
            n = addNodes(n);
        }
        topID = existingNodes[n];//store the id of the top node
        generatedTrees[gen] = topID;//store the id of the top node of the generated tree
    }

    return topID;
}


void CanonicalTree::expend(int nbLevels, int value, int x, int y, int z) {
    if(nbLevels<1) return;//nothing to do
    level += nbLevels;//update level
    length = 1<<level;//update length
    ratio = length/width;//update ratio

    Node actual = nodes[topID];//save actual top node
    topID = generate(level, value);//generate desired level of same value and set topID so the tree becomes it
    setNode(x,y,z,actual);
}

void CanonicalTree::expendCenter(int nbLevels, int value) {
    if(nbLevels<1) return;//nothing to do
    level += nbLevels;//update level
    length = 1<<level;//update length
    ratio = length/width;//update ratio

    Node actual = nodes[topID];//save actual top node
    topID = generate(level, value);//generate desired level of same value and set topID so the tree becomes it

    int sh = 1<<nbLevels, ones = 0b11111111111111111111111111111111;//bit shift
    for (int z = 0; z < 2; ++z) {
        for (int y = 0; y < 2; ++y) {
            for (int x = 0; x < 2; ++x) {
                setNode((ones*!x)^sh,(ones*!y)^sh,(ones*!z)^sh,nodes[actual.subNodes[z][y][x]]);
            }
        }
    }
}

int CanonicalTree::get(int x, int y, int z) {
    if(x<0 || y<0 || z<0 || x>length || y>length || z>length){//out of the tree
        return -1;
    }

    Node n = nodes[topID];
    for(int i = n.level-1;i>=0;i--){
        if(n.value != -1) return n.value;
        n = nodes[n.subNodes[z>>i&1][y>>i&1][x>>i&1]];
    }
    return n.value;
}
int CanonicalTree::getf(float x, float y, float z) {
    return get((x+px)*ratio, (y+py)*ratio, (z+pz)*ratio);
}

Node CanonicalTree::getNode(int x, int y, int z, int nodeLevel) {
    if(x<0 || y<0 || z<0 || x>length || y>length || z>length){//out of the tree
        return Node();
    }

    Node n = nodes[topID];
    int id, depth = n.level-1-nodeLevel;

    for(int i = depth;i>=0;i--){
        id = n.subNodes[z>>i&1][y>>i&1][x>>i&1];
        n = nodes[id];
    }

    return n;
}
Node CanonicalTree::getNodef(float x, float y, float z, int nodeLevel) {
    return getNode((x+px)*ratio, (y+py)*ratio, (z+pz)*ratio, nodeLevel);
}

Node CanonicalTree::getCenter(int nbSubLevel) {
    Node n;
    n.level = level-nbSubLevel;
    int sh = 1<<nbSubLevel, ones = 0b11111111111111111111111111111111;//bit shift
    for (int z = 0; z < 2; ++z) {
        for (int y = 0; y < 2; ++y) {
            for (int x = 0; x < 2; ++x) {
                n.subNodes[z][y][x] = existingNodes[getNode((ones*!x)^sh,(ones*!y)^sh,(ones*!z)^sh, n.level-1)];//level-1
                // because the subnodes are set so the nodes of level-1
            }
        }
    }
    n.value = checkSameValue(n);
    return n;
}

int CanonicalTree::set(int x, int y, int z, int value) {
    if(x<0 || y<0 || z<0 || x>length || y>length || z>length){//out of the tree
        return -1;
    }

    if(level == 0){//if the tree is only a leaf, then just change the value of the leaf
        topID = addNode(Node(value));
        return value;
    }

    Node n = nodes[topID];
    int id, depth = n.level-1;
    int ids[depth+2];//store each ids while descending the tree to climb after set
    ids[depth+1] = topID;
    for(int i = depth;i>=0;i--){
        id = n.subNodes[z>>i&1][y>>i&1][x>>i&1];
        ids[i] = id;
        n = nodes[id];
        if(n.value == value) return value;
    }//if the "for" finishes then the value doesn't already exist so it must be set

    Node n1(value);//create the new node
    for(int i = 0;i<=depth;i++){
        //start at index i+1 because we don't want the old leaf
        n = nodes[ids[i+1]];//copy the node so we can apply changes on it without change the stored nodes
        n.subNodes[z>>i&1][y>>i&1][x>>i&1] = addNode(n1);//change the subnode with the new node
        n.value = checkSameValue(n);//check if all subvalues are the same, else the value will be -1
        n1 = n;
    }
    topID = addNode(n1);

    return value;
}
int CanonicalTree::setf(float x, float y, float z, int value) {
    return set((x+px)*ratio, (y+py)*ratio, (z+pz)*ratio, value);
}

Node CanonicalTree::setNode(int x, int y, int z, Node node) {
    if(x<0 || y<0 || z<0 || x>length || y>length || z>length){//out of the tree
        return Node();
    }

    Node n = nodes[topID];
    int id, depth = n.level-1-node.level;

    if(depth == -1){
        topID = addNode(node);
        return node;
    }
    if(depth < -1){
        return Node();//can't be set because larger than the avtual tree
    }

    int ids[depth+2];//store each ids while descending the tree to climb after set
    ids[depth+1] = topID;
    for(int i = depth;i>=0;i--){
        id = n.subNodes[z>>i&1][y>>i&1][x>>i&1];
        ids[i] = id;
        n = nodes[id];
        if(n.value == node.value && node.value != -1) return node;//already the same, stop here
    }//if the "for" finishes then the value doesn't already exist so it must be set

    Node n1(node);//create the new node
    for(int i = 0;i<=depth;i++){
        //start at index i+1 because we don't want the old leaf
        n = nodes[ids[i+1]];//copy the node so we can apply changes on it without change the stored nodes
        n.subNodes[z>>i&1][y>>i&1][x>>i&1] = addNode(n1);//change the subnode with the new node
        n.value = checkSameValue(n);//check if all subvalues are the same, else the value will be -1
        n1 = n;
    }
    topID = addNode(n1);

    return node;
}
Node CanonicalTree::setNodef(float x, float y, float z, Node node){
    return setNode((x+px)*ratio, (y+py)*ratio, (z+pz)*ratio, node);
}

int CanonicalTree::getLevel() {
    return level;
}

int CanonicalTree::checkSameValue(Node const &node) {

    int val = nodes[node.subNodes[0][0][0]].value;
    bool eq = true;

    for (int x = 0; x < 8; ++x) {
        if(val != nodes[node.subNodes[x>>2&1][x>>1&1][x&1]].value){
            eq = false;
            break;
        }
    }

    if(eq) return val;
    else return -1;
}

int CanonicalTree::nbNodes() {
    return nodes.size();
}

int CanonicalTree::getLength() {
    return length;
}

int CanonicalTree::getTopID() {
    return topID;
}

std::vector<Node> CanonicalTree::getNodes() {
    return nodes;
}


DataRay CanonicalTree::raycast(float x, float y, float z, float dx, float dy, float dz){
    if(x<0 || y<0 || z<0 || x>length || y>length || z>length){//out of the tree
        return DataRay();
    }

    //first normalize direction
    float norm = sqrt(dx*dx + dy*dy + dz*dz);
    dx /= norm;
    dy /= norm;
    dz /= norm;

    float epsilon = 1e-3;
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

        float tTab[] = {dx==0?-1:(ox-x)/dx,dy==0?-1:(oy-y)/dy,dz==0?-1:(oz-z)/dz,
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

        if(x<0 || y<0 || z<0 || x>length || y>length || z>length){//out of the tree
            node.value = -1;//set value to -1 to say oout of the tree
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
DataRay CanonicalTree::raycastf(float x, float y, float z, float dx, float dy, float dz) {
    DataRay data = raycast((x+px)*ratio, (y+py)*ratio, (y+py)*ratio, dx, dy, dz);
    data.x = data.x/ratio - px;
    data.y = data.y/ratio - py;
    data.z = data.z/ratio - pz;

    return data;
}
