#ifndef _AvlTree_
#define _AvlTree_

typedef struct AvlTree *AvlTreePtr;

int AvlTree_Create(AvlTreePtr *tree);
int AvlTree_Insert(AvlTreePtr tree,char *data);
int AvlTree_NumWords(AvlTreePtr tree);
int AvlTree_Search(AvlTreePtr tree,char *wanted);
int AvlTree_Destroy(AvlTreePtr *tree);

#endif