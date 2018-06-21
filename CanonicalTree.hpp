//
// Created by Nathan Touroux on 29/05/2018.
//

#ifndef CELLAUTENGINE_TREE_HPP
#define CELLAUTENGINE_TREE_HPP


#include <vector>
#include <unordered_set>
#include <unordered_map>

#include "Node.hpp"

//WARNING: COMMENTS ARE AT THE RIGHT OR JUST UNDER THE LINE IT REFERS TO

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

MAKE_HASHABLE_TT(std::pair, t.first, t.second)

struct DataRay{
    float x=0,y=0,z=0;//position of the collision
    float nx=0,ny=0,nz=0;//normal of the collision
    int value=-1;
    float distance=0;//total distance from the origine of the ray to the collision

    operator int(){
        return value;
    }
    operator float(){
        return distance;
    }
};

class CanonicalTree {
private:
    static std::unordered_map<Node, int /* id */> existingNodes;
    static std::unordered_map<std::pair<int,int>, int /* id */> generatedTrees;
    //static std::unordered_map<unsigned int, Tree<A>> memoizedEmpty;
    //static std::unordered_map<Tree<A>, Tree<A>> memoizedNext;
    //static std::unordered_map<Tree<A>, std::vector<std::vector<A>>> memoizedGet;

    static std::vector<Node> nodes;
    int topID;//id of the top node of the tree
    int level, length;

    float width, px,py,pz, ratio;//to have a floating number coordinate system the width of the tree (as a cube) must be define
    // and the position of the origine. then to find the leaf at coordinate (x,y,z) the calculus to come back in binary
    // coordinate is ((x,y,z)+(px,py,pz))*(1<<getLevel())/width    where 1<<getLevel() correspond to the power of 2 of
    // the level of the tree so the integer length
    // To have the center of the tree as the origine (0,0,0) set (px,py,pz)=(width/2,width/2,width/2)
    // then in binary it becomes ((0,0,0)+(width/2,width/2,width/2))*(1<<getLevel())/width = (1/2,1/2,1/2)*(1<<getLevel())
    // so half the tree in each coordinate
    // -> each method that as the same name of another except it finishes with an "f" use the floating coordinate system
    // -> by default the origine is the same as the tree

    int addNode(Node const &n);
    Node addNodes(Node const &node);

    Node getNode(int x, int y, int z, int nodeLevel = 0);
    Node getNodef(float x, float y, float z, int nodeLevel = 0);
    Node getCenter(int nbSubLevel = 1);
    Node setNode(int x, int y, int z, Node node);//the coordinate correspond to the position of the node to be set as it
    // is the smallest element: for a tree of level 3 so 8x8x8 cubes, to set a node of level 1 so 2x2x2 the cooridanate
    // are in the range [0,2^(3-1)[ so [0,4[ (with inclusive bracket [0,3] )
    // to set it at the origine the coordinate would be (0,0,0) and at the opposite corner (3,3,3)
    Node setNodef(float x, float y, float z, Node node);

    int checkSameValue(Node const &node);
public:
    explicit CanonicalTree(int level = 0, int value = 0);
    //create a tree of indicated level with all same values
    CanonicalTree(int level, int value, float width);
    CanonicalTree(int level, int value, float width, float px, float py, float pz);
    CanonicalTree(const CanonicalTree &tree);
    ~CanonicalTree();

    int generate(int level = 0, int value = 0);//create a tree of indicated level with all same values and return the id of the top node
    void expend(int nbLevels = 1, int value = 0, int x = 0, int y = 0, int z = 0);//expends with actual tree at position
    // (x,y,z) with same coordinate principle as in setNode()
    void expendCenter(int nbLevels = 1, int value = 0);

    int getLevel();
    int nbNodes();
    int getLength();

    int get(int x, int y, int z);//return the value
    int getf(float x, float y, float z);//return the value
    int set(int x, int y, int z, int value);//set the value and return it
    int setf(float x, float y, float z, int value);//set the value and return it
    //TODO: add security if chosen location is out of the tree

    DataRay raycast(float x, float y, float z, float dx, float dy, float dz);
    DataRay raycastf(float x, float y, float z, float dx, float dy, float dz);
};


#endif //CELLAUTENGINE_TREE_HPP
