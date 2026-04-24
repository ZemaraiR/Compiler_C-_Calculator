
#ifndef ASTNODE_H
#define ASTNODE_H

#include <string>
#include <vector>
#include <iostream>
using namespace std;

struct ASTNode 
{
    string label;
    vector<ASTNode*> children;

    ASTNode(string l);
    ~ASTNode();
    void addChild(ASTNode* child);
    void print(string prefix, bool isLast);
};

#endif
