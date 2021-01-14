#pragma once

#define NODE_RED 0
#define NODE_BLACK 1

struct TreeNode
{
    int color;     // 节点的颜色
    int value;     // 节点的值
    int isLeaf;    // 是否是叶子节点
    struct TreeNode *lchild, *rchild;
    struct TreeNode *parent;
};

struct RBTree 
{
    struct TreeNode* proot;
    unsigned int size;
};

struct RBTree* newRBTree();
struct TreeNode* newLeafNode();
int isLeftChild(struct TreeNode* pnode);
int isLeaf(struct TreeNode* pnode);
void addLeafChildren(struct TreeNode* pnode);
void changeToLeaf(struct TreeNode* pnode);
struct TreeNode* getUncle(struct TreeNode* pnode);
struct TreeNode* getBrother(struct TreeNode* pnode);
struct TreeNode* getNearNephew(struct TreeNode* pnode);
struct TreeNode* getFarNephew(struct TreeNode* pnode);
void leftRotate(struct TreeNode* pnode);
void rightRotate(struct TreeNode* pnode);
void insertFixup(struct RBTree* ptree, struct TreeNode* pnode);
void insertElement(struct RBTree* ptree, int newvalue);
void removeFixup(struct RBTree* ptree, struct TreeNode* pnode);
void removeElement(struct RBTree* ptree, int target);
void printTree(struct RBTree* ptree);