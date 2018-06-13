//
// Created by Nathan Touroux on 29/05/2018.
//

#include <iostream>
#include "CanonicalTree.hpp"

std::unordered_map<Node, int> CanonicalTree::existingNodes;
std::vector<Node> CanonicalTree::nodes;
std::unordered_map<std::pair<int,int>, int> CanonicalTree::generatedTrees;

CanonicalTree::CanonicalTree(int level, int value){
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
}

CanonicalTree::CanonicalTree(const CanonicalTree &tree): topID(tree.topID) {

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
//TODO maybe remove:
/*Node CanonicalTree::addNodes(const Node nodes[2][2][2]){
    Node n;
    n.level = nodes[0][0][0].level+1;

    int val = nodes[0][0][0].value;
    bool eq = true;

    for (int z = 0; z < 2; ++z) {
        for (int y = 0; y < 2; ++y) {
            for (int x = 0; x < 2; ++x) {
                n.subNodes[z][y][x] = addNode(nodes[z][y][x]);
                if(val != nodes[z][y][x].value) eq = false;
            }
        }
    }

    if(eq) n.value = val;

    addNode(n);
    return n;
}*/

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

void CanonicalTree::expend(int nbLevels, int value) {
    //TODO
}

int CanonicalTree::get(int x, int y, int z) {
    Node n = nodes[topID];
    for(int i = n.level-1;i>=0;i--){
        if(n.value != -1) return n.value;
        n = nodes[n.subNodes[z>>i&1][y>>i&1][x>>i&1]];
    }
    return n.value;
}

int CanonicalTree::set(int x, int y, int z, int value) {
    Node n = nodes[topID];
    int id, depth = n.level-1;
    int ids[depth+2];//store each ids while descending the tree to climb after set
    ids[depth+1] = existingNodes[n];
    for(int i = depth;i>=0;i--){
        id = n.subNodes[z>>i&1][y>>i&1][x>>i&1];
        ids[i] = id;
        n = nodes[id];
        if(n.value == value) return value;
    }//if the "for" finished then the value doesn't already exist so it must be set

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

int CanonicalTree::getLevel() {
    return nodes[topID].level;
}

int CanonicalTree::checkSameValue(Node const &node) {

    int val = node.value;
    bool eq = true;

    for (int z = 0; z < 2; ++z) {
        for (int y = 0; y < 2; ++y) {
            for (int x = 0; x < 2; ++x) {
                if(val != nodes[node.subNodes[z][y][x]].value) eq = false;
            }
        }
    }

    if(eq) return val;
    else return -1;
}

int CanonicalTree::nbNodes() {
    return nodes.size();
}
