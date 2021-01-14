#include <stdio.h>
#include "rbtree.h"

int main()
{
    struct RBTree* ptree = newRBTree();
    // int data[] = {1,2,3,4,5,6,7,8,9};
    int data[] = {13,2,5,2,1,7,1,3,4,9,12,2,31,17,10,7,7};
    int rdata[] = {8,7,6,2,13,9};
    int n = sizeof(data)/sizeof(data[0]), rn = sizeof(rdata)/sizeof(rdata[0]);
    for (int i = 0; i < n; ++i)
        insertElement(ptree, data[i]);
    printTree(ptree);
    printf("\n");  
    for (int i = 0; i < rn; ++i)
        removeElement(ptree, rdata[i]);
    printTree(ptree);
}