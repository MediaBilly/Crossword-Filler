#ifndef _AvlTree_
#define _AvlTree_

typedef struct AvlTree *AvlTreePtr;

int AvlTree_Create(AvlTreePtr *tree);
int AvlTree_Insert(AvlTreePtr tree,char *data);
int AvlTree_NumWords(AvlTreePtr tree);
int AvlTree_Search(AvlTreePtr tree,char *wanted);
int AvlTree_NumWordsWithPattern(AvlTreePtr tree,char *pattern);
int AvlTree_PatternExists(AvlTreePtr tree,char *pattern);
int AvlTree_NewSession(AvlTreePtr tree);
char *AvlTree_NextWordOfSession(AvlTreePtr tree,int sid);
int AvlTree_Destroy(AvlTreePtr *tree);

#endif