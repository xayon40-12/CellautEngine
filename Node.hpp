//
// Created by Nathan Touroux on 30/05/2018.
//

#ifndef CELLAUTENGINE_NODE_HPP
#define CELLAUTENGINE_NODE_HPP

#include "Hashable.hpp"

struct Node {
    explicit Node(int val = -1);
    Node(Node const &n);
    ~Node();

    bool operator==(Node const &n) const;

    int subNodes[2][2][2];//[z][y][x]
    //all subnodes MUST have the same level
    int value;
    int level;//a leaf is level 0, then each branch above is +1
};

MAKE_HASHABLE(Node, t.level, t.value, t.subNodes[0][0][0], t.subNodes[0][0][1],
t.subNodes[0][1][0], t.subNodes[0][1][1],
t.subNodes[1][0][0], t.subNodes[1][0][1],
t.subNodes[1][1][0], t.subNodes[1][1][1])


#endif //CELLAUTENGINE_NODE_HPP
