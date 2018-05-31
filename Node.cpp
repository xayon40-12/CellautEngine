//
// Created by Nathan Touroux on 30/05/2018.
//

#include "Node.hpp"

Node::Node(int val): subNodes{0}, value(val), level(0) {}
Node::Node(Node const &n) = default;
Node::~Node() = default;

bool Node::operator==(Node const &n) const{
    return value == n.value && level == n.level &&
           n.subNodes[0][0][0] == subNodes[0][0][0] && n.subNodes[0][0][1] == subNodes[0][0][1] &&
           n.subNodes[0][1][0] == subNodes[0][1][0] && n.subNodes[0][1][1] == subNodes[0][1][1] &&
           n.subNodes[1][0][0] == subNodes[1][0][0] && n.subNodes[1][0][1] == subNodes[1][0][1] &&
           n.subNodes[1][1][0] == subNodes[1][1][0] && n.subNodes[1][1][1] == subNodes[1][1][1];
}
