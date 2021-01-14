#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "rbtree.h"

// 红黑树特性
// (1) 每个节点或者是黑色，或者是红色。
// (2) 根节点是黑色。
// (3) 每个叶子节点是黑色。 [注意：这里叶子节点，是指为空的叶子节点！]
// (4) 如果一个节点是红色的，则它的子节点必须是黑色的。
// (5) 从一个节点到该节点的子孙节点的所有路径上包含相同数目的黑节点。
// 参考: 
// (1) https://www.cnblogs.com/skywang12345/p/3245399.html
// (2) https://www.cnblogs.com/gcheeze/p/11186806.html

struct RBTree* newRBTree()
{
    struct RBTree* ptree;
    ptree = (struct RBTree*)malloc(sizeof(struct RBTree));

    ptree->proot = (struct TreeNode*)malloc(sizeof(struct TreeNode));
    ptree->proot->color = NODE_BLACK;
    ptree->proot->lchild = ptree->proot->rchild = ptree->proot->parent = NULL;
    ptree->proot->isLeaf = 1;  // 初始根节点未叶子节点

    ptree->size = 0;

    return ptree;
}

struct TreeNode* newLeafNode()
{
    struct TreeNode* pleaf;
    pleaf = (struct TreeNode*)malloc(sizeof(struct TreeNode));

    pleaf->color = NODE_BLACK;
    pleaf->isLeaf = 1;
    pleaf->parent = pleaf->lchild = pleaf->rchild = NULL;

    return pleaf;
}

void addLeafChildren(struct TreeNode* pnode)
{
    struct TreeNode *lleaf, *rleaf;
    lleaf = newLeafNode();
    rleaf = newLeafNode();

    pnode->lchild = lleaf;
    lleaf->parent = pnode;
    pnode->rchild = rleaf;
    rleaf->parent = pnode;
}

void changeToLeaf(struct TreeNode* pnode)
{
    free(pnode->lchild);
    free(pnode->rchild);
    pnode->lchild = pnode->rchild = NULL;
    pnode->color = NODE_BLACK;
    pnode->isLeaf = 1;
}

int isLeftChild(struct TreeNode* pnode)
{
    if (pnode->parent && pnode == pnode->parent->lchild)
        return 1;
    return 0;
}

int isLeaf(struct TreeNode* pnode)
{
    return pnode->isLeaf;
}

// 获取叔叔节点
struct TreeNode* getUncle(struct TreeNode* pnode)
{
    struct TreeNode* grandpa = pnode->parent->parent;
    if (isLeftChild(pnode->parent))
        return grandpa->rchild;
    else
        return grandpa->lchild;
}

// 获取兄弟节点
struct TreeNode* getBrother(struct TreeNode* pnode)
{
    if (isLeftChild(pnode))
        return pnode->parent->rchild;
    return 
        pnode->parent->lchild;
}

// 获取较近的侄子节点
struct TreeNode* getNearNephew(struct TreeNode* pnode)
{
    struct TreeNode* pbrother = getBrother(pnode);
    return isLeftChild(pnode)?pbrother->lchild:pbrother->rchild;
}

// 获取较远的侄子节点
struct TreeNode* getFarNephew(struct TreeNode* pnode)
{
    struct TreeNode* pbrother = getBrother(pnode);
    return isLeftChild(pnode)?pbrother->rchild:pbrother->lchild;
}

// 左旋
void leftRotate(struct TreeNode* pnode)
{
    struct TreeNode* pa = pnode->parent;
    struct TreeNode* rchild = pnode->rchild;
    struct TreeNode* rlchild = rchild->lchild;

    if (pa)
    {
        if (isLeftChild(pnode))
            pa->lchild = rchild;
        else
            pa->rchild = rchild;
    }
    rchild->parent = pa;
    pnode->rchild = rlchild;
    rlchild->parent = pnode;
    rchild->lchild = pnode;
    pnode->parent = rchild;
}

// 节点右旋
void rightRotate(struct TreeNode* pnode)
{
    struct TreeNode* pa = pnode->parent;
    struct TreeNode* lchild = pnode->lchild;
    struct TreeNode* lrchild = lchild->rchild;

    if (pa)
    {
        if (isLeftChild(pnode))
            pa->lchild = lchild;
        else
            pa->rchild = lchild;
    }
    lchild->parent = pa;
    pnode->lchild = lrchild;
    lrchild->parent = pnode;
    lchild->rchild = pnode;
    pnode->parent = lchild;
}

void insertFixup(struct RBTree* ptree, struct TreeNode* pnode)
{
    struct TreeNode* pcur = pnode;
    while (pcur->color == NODE_RED)
    {
        // case 1: 当前节点为树的根节点
        // 操作：直接将该节点颜色置为黑色
        if (pcur == ptree->proot)
        {
            pcur->color = NODE_BLACK;
            break;
        }

        // case 2: 插入节点的父节点为黑色
        // 操作：什么也不用做：不违反任何红黑树特性
        if (pcur->parent->color == NODE_BLACK)
            break;

        // case 3: 插入节点的父节点为红色
        // 操作：需要重新旋转与着色(违反特性4)，核心：将红色节点移动到根节点，然后染黑
        struct TreeNode* grandpa = pcur->parent->parent;
        struct TreeNode* puncle = getUncle(pcur);

        // case 3.1: 叔叔节点为红色(祖父必然为黑色)
        // 操作：将父亲节点和叔叔节点染黑，祖父染红，然后将祖父节点设置为当前节点
        if (puncle->color == NODE_RED)
        {
            pcur->parent->color = puncle->color = NODE_BLACK;
            grandpa->color = NODE_RED;
            pcur = grandpa;
        }

        // case 3.2: 叔叔为黑色，当前节点和父节点均为左孩子或均为右孩子
        // 操作：将祖父节点染红，父节点染黑，祖父节点右旋或左旋
        else if (isLeftChild(pcur) == isLeftChild(pcur->parent))
        {
            grandpa->color = NODE_RED;
            pcur->parent->color = NODE_BLACK;
            isLeftChild(pcur)?rightRotate(grandpa):leftRotate(grandpa);
            if (ptree->proot == grandpa)  // 重新调整树的根节点
                ptree->proot = grandpa->parent;
            break;
        }

        // case 3.3: 叔叔为黑色，当前节点为右孩子而父节点为左孩子或者相反
        // 操作：父节点左旋，然后将父节点设置为当前节点，转换为case3.2
        else
        {
            struct TreeNode* pa = pcur->parent;
            isLeftChild(pcur)?rightRotate(pa):leftRotate(pa);
            pcur = pa;
        }
    }
}

// 向红黑树中插入元素
void insertElement(struct RBTree* ptree, int newvalue)
{
    // 找到插入位置
    struct TreeNode* pcur = ptree->proot;
    while (!isLeaf(pcur))
    {
        if (newvalue == pcur->value)  // 存在重复值
            return;
        if (newvalue > pcur->value)
            pcur = pcur->rchild;
        else
            pcur = pcur->lchild;
    }
    
    pcur->color = NODE_RED;
    pcur->isLeaf = 0;
    pcur->value = newvalue;
    addLeafChildren(pcur);

    insertFixup(ptree, pcur);
    ptree->size++;
}

void removeFixup(struct RBTree* ptree, struct TreeNode* pnode)
{
    struct TreeNode* pcur = pnode;
    struct TreeNode* pbrother, *pNearNephew, *pFarNephew;
    while (pcur != ptree->proot)
    {
        // case 1: 兄弟节点为红色(父节点必为黑色)
        // 操作: 通过旋转与重新着色使得其兄弟节点变为黑色(转换为case 2)
        // 具体: 父节点染红，兄弟节点染黑，父节点旋转
        pbrother = getBrother(pcur);
        if (pbrother->color == NODE_RED)
        {
            pcur->parent->color = NODE_RED;
            pbrother->color = NODE_BLACK;
            isLeftChild(pcur)?leftRotate(pcur->parent):rightRotate(pcur->parent);
        }

        // case 2: 兄弟节点为黑色
        // 2.1: 较近的侄子节点为红色
        // 操作: 兄弟节点染红，近侄子染黑，兄弟节点旋转，转换为case 2.2
        pNearNephew = getNearNephew(pcur);
        if (pNearNephew->color == NODE_RED)
        {
            pbrother = getBrother(pcur);
            pNearNephew->color = NODE_BLACK;
            pbrother->color = NODE_RED;
            isLeftChild(pcur)?rightRotate(pbrother):leftRotate(pbrother);
        }

        // case 2.2: 较远的侄子节点为红色(有红色节点的分支可以借出去一个黑色，再把该红色节点染黑)
        // 操作: 兄弟节点和父节点颜色互换，父节点旋转,再把远侄子节点染黑
        pFarNephew = getFarNephew(pcur);
        if (pFarNephew->color == NODE_RED)
        {
            pbrother = getBrother(pcur);
            int tmpcolor = pcur->parent->color;
            pcur->parent->color = pbrother->color;
            pbrother->color = tmpcolor;
            isLeftChild(pcur)?leftRotate(pcur->parent):rightRotate(pcur->parent);

            pFarNephew->color = NODE_BLACK;
            break;
        }

        // case 2.3: 两个侄子节点均为黑色，父节点为红色
        // 操作: 兄弟节点染红，父节点染黑，然后删除当前节点
        if (pcur->parent->color == NODE_RED)
        {
            pbrother = getBrother(pcur);
            pcur->parent->color = NODE_BLACK;
            pbrother->color = NODE_RED;
            break;
        }
        
        // case 2.4: 两个侄子节点均为黑色，父节点也为黑色
        // 操作: 兄弟节点染红，父节点上调
        pcur = pcur->parent;
    }
}

// 从红黑树中删除元素
void removeElement(struct RBTree* ptree, int target)
{
    // 找到删除元素的位置
    struct TreeNode* pcur = ptree->proot;
    if (pcur == NULL) return;
    while (!isLeaf(pcur) && target != pcur->value)
        pcur = target>pcur->value?pcur->rchild:pcur->lchild;

    // 未找到需要删除的目标值
    if (isLeaf(pcur))
        return;

    // case 1: 待删除节点的左右孩子均为非叶子节点
    // 操作: 将该节点的直接后继节点的值复制到该节点，然后删除后继节点(转化为后面的情况)
    if (!isLeaf(pcur->lchild) && !isLeaf(pcur->rchild))
    {
        // 找直接后继节点
        struct TreeNode* psucc = pcur->rchild;
        while (!isLeaf(psucc->lchild))
            psucc = psucc->lchild;
        pcur->value = psucc->value;
        pcur = psucc;
    }

    // 接下来的情况均为只有一个非叶孩子节点或者两个孩子节点都是叶子节点
    // case 2: 删除节点颜色为红，两个孩子都是叶子节点(不可能存在只有一个孩子是叶子节点)
    // 操作: 直接删除，不违背红黑树特性
    if (pcur->color == NODE_RED)
    {
        changeToLeaf(pcur);
        ptree->size--;
        return;
    }

    // case 3: 删除节点的颜色为黑
    // case 3.1: 左孩子或者右孩子为非叶子节点(两个孩子均为非叶子节点即为case 1)
    // 操作: 直接用该非叶子孩子节点的值替换掉删除节点，然后删除该非叶子孩子节点
    if (!isLeaf(pcur->lchild) || !isLeaf(pcur->rchild))
    {
        struct TreeNode* pchild = pcur->rchild;
        if (!isLeaf(pcur->lchild))
            pchild = pcur->lchild;
        pcur->value = pchild->value;
        changeToLeaf(pchild);
        ptree->size--;
        return;
    }

    // case 3.2: 左右孩子均为叶子节点(删除该节点，该分支黑色节点少1，只能从隔壁“借”啦)
    // case 3.2.1: 删除节点为根节点
    if (pcur == ptree->proot)
    {
        changeToLeaf(pcur);
        free(pcur);
        ptree->proot = NULL;
        ptree->size = 0;
        return;
    }

    // case 3.2.2: 删除节点为非根节点
    // 直接删除，然后通过旋转和重新染色使其符合红黑树特性
    changeToLeaf(pcur);
    ptree->size--;
    removeFixup(ptree, pcur);
}

void printTree(struct RBTree* ptree)
{
    printf("rbtree size: %u\n", ptree->size);
    if (ptree->size == 0) return;
    struct TreeNode** queue;
    queue = (struct TreeNode**)malloc(sizeof(struct TreeNode*)*ptree->size);
    queue[0] = ptree->proot;
    int front = 0, rear = 1;
    int deep = 0;
    while (front < rear)
    {
        deep++;
        int num = rear-front;
        printf("%d floor(%d): ", deep, num);
        for (int i = 0; i < num; ++i)
        {
            struct TreeNode* pnode = queue[front++];
            printf(" %d(%s)[%d %d]"+!i, pnode->value, pnode->color==NODE_RED?"red":"black",
                pnode->lchild->isLeaf?-1:pnode->lchild->value, pnode->rchild->isLeaf?-1:pnode->rchild->value);
            if (!pnode->lchild->isLeaf)
                queue[rear++] = pnode->lchild;
            if (!pnode->rchild->isLeaf)
                queue[rear++] = pnode->rchild;
        }
        printf("\n");
    }
    free(queue);
}