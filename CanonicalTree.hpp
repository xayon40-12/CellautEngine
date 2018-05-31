//
// Created by Nathan Touroux on 29/05/2018.
//

#ifndef CELLAUTENGINE_TREE_HPP
#define CELLAUTENGINE_TREE_HPP


#include <vector>
#include <unordered_set>
#include <unordered_map>

#include "Node.hpp"


//direction convetion:
//each cube is divided in 8 subcubes (each node of the tree is a cube)
//       y
//       | ___ ___
//       /___/___/|
//     /___/___/| |
//    |   |   | |/|
//    |___|___|/| | __ x
//    |   |   | |/
//    |___|___|/
//   /
//  z
//
//The bottom left back subcube is the origine with coordinate (0,0,0). To move a cube further in a direction add 1 to the
// coresponding coordinate
//
//Thus with the binary value of a number it is simple to move in subtrees, reading from left to right a 1 move in the positive
// direction of the coordinate, a 0 move to the negative direction
//in 1D:
// for the tree |0|1|2|3|
// position 2 (10 in binary) so right then left
// 1) |   |***|
// 2)     |*| |
//Then it correspond:
// 3) | | |2| |


class CanonicalTree {
private:
    static std::unordered_map<Node, int /* id */> existingNodes;
    //static std::unordered_map<unsigned int, Tree<A>> memoizedEmpty;
    //static std::unordered_map<Tree<A>, Tree<A>> memoizedNext;
    //static std::unordered_map<Tree<A>, std::vector<std::vector<A>>> memoizedGet;

    std::vector<Node> *nodes;
    int topID;//id of the top node of the tree

    int addNode(Node const &n);
    Node addNodes(const Node nodes[2][2][2]);
    Node addNodes(Node const &node);

    int checkSameValue(Node const &node);
public:
    explicit CanonicalTree(int level = 0, int value = 0);//create a leaf
    CanonicalTree(const CanonicalTree &tree);
    ~CanonicalTree();

    void expend(int nbLevels = 1, int value = 0);

    int getLevel();

    int get(int x, int y, int z);//return the value
    int set(int x, int y, int z, int value);//set the value and return it
};


#endif //CELLAUTENGINE_TREE_HPP
