#include "ASTNode.h"

ASTNode::ASTNode(string l) 
{
    label = l;
}

ASTNode::~ASTNode() 
{
    for (int i = 0; i < children.size(); i++)
    {
        delete children[i];
    }
}

void ASTNode::addChild(ASTNode* child) 
{
    if (child != nullptr)
    {
        children.push_back(child);
    } 
}

void ASTNode::print(string prefix, bool isLast) 
{
    cout << prefix;
    if (isLast)
    {
       cout << "└── "; 
    }
    else
    {
       cout << "├── "; 
    }
    cout << label << endl;

    for (int i = 0; i < children.size(); i++) 
    {
        string newPrefix = prefix;
        if (isLast)
        {
            newPrefix += "    ";
        }
        else
        {
            newPrefix += "│   ";
        }
        children[i]->print(newPrefix, i == children.size() - 1);
    }
}