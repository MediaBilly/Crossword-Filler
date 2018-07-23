#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AVL_Tree.h"

typedef struct TreeNode *TreeNodePtr;

struct TreeNode
{
	TreeNodePtr left,right,parent;
	char *data;
};

struct AvlTree
{
	TreeNodePtr root;
	int count;
};

int AvlTree_Create(AvlTreePtr *tree)
{
	if((*tree = (AvlTreePtr)malloc(sizeof(struct AvlTree))) == NULL) //Memory problem check
		return 0;
	(*tree)->root = NULL;
	(*tree)->count = 0;
	return 1;
}

int Insert(TreeNodePtr *dest,TreeNodePtr node,TreeNodePtr parent)
{
	if(*dest != NULL)
	{
		if(strcmp(node->data,(*dest)->data) < 0)//Insert at left subtree
			return Insert(&((*dest)->left),node,*dest);
		else if(strcmp(node->data,(*dest)->data) > 0)//Insert at right subtree
			return Insert(&((*dest)->right),node,*dest);
		else
			return 0;//Already exists
	}
	else
	{
		*dest = node;//Successfull insertion
		(*dest)->parent = parent;
		return 1;
	}
}

int AvlTree_Insert(AvlTreePtr tree,char *data)
{
	TreeNodePtr tmp;
	if((tmp = (TreeNodePtr)malloc(sizeof(struct TreeNode))) == NULL) //Memory problem check
		return -1;
	tmp->left = tmp->right = NULL;
	if((tmp->data = (char*)malloc(strlen(data) + 1)) == NULL) //Memory problem check
		return -1;
	strcpy(tmp->data,data);
	if(!Insert(&(tree->root),tmp,NULL)) //Content already exists
		return 0;
	tree->count++;
	return 1;
}

int AvlTree_NumWords(AvlTreePtr tree)
{
	return tree->count;
}

int Search(TreeNodePtr node,char *wanted)
{
	if(node != NULL)
	{
		if(strcmp(wanted, node->data) < 0) //Search left subtree
			return Search(node->left,wanted);
		else if(strcmp(wanted, node->data) > 0) //Search right subtree
			return Search(node->right,wanted);
		else
			return 1;//Found!
	}
	else
		return 0;//Not found
}

int AvlTree_Search(AvlTreePtr tree,char *wanted)
{
	return Search(tree->root,wanted);
}

void Destroy(TreeNodePtr node)
{
	if(node != NULL)
	{
		Destroy(node->left);
		Destroy(node->right);
		free(node->data);
		free(node);
	}
}

int AvlTree_Destroy(AvlTreePtr *tree)
{
	if(*tree == NULL) //Not initialized
		return 0;
	Destroy((*tree)->root);
	free(*tree);
	*tree = NULL;
	return 1;
}