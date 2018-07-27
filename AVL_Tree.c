/* 
	The implementation of the Avl Tree data structure.
   	I did not implement the function to delete a specific node because it is not used in this program.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AVL_Tree.h"

#define MAX(A,B) (((A) > (B)) ? (A) : (B))
#define ABS(X) (((X) < 0) ? (-(X)) : (X))

typedef struct TreeNode *TreeNodePtr;

struct TreeNode
{
	TreeNodePtr left,right,parent;
	char *data;
	int lHeight,rHeight,height,diff;
};

struct AvlTree
{
	TreeNodePtr root;
	TreeNodePtr *session;
	int count,sessions;
};

int AvlTree_Create(AvlTreePtr *tree)
{
	if((*tree = (AvlTreePtr)malloc(sizeof(struct AvlTree))) == NULL) //Memory problem check
		return 0;
	(*tree)->root = NULL;
	(*tree)->session = NULL;
	(*tree)->count = (*tree)->sessions = 0;
	return 1;
}

void RotateLeft(TreeNodePtr *root)
{
	TreeNodePtr tmp = *root,parent = (*root)->parent;
	*root = (*root)->right;
	tmp->right = (*root)->left;
	(*root)->left = tmp;

	(*root)->left->rHeight = (*root)->left->right == NULL ? 0 : (*root)->left->right->height;
	(*root)->left->height = MAX((*root)->left->lHeight,(*root)->left->rHeight) + 1;
	(*root)->left->diff = (*root)->left->rHeight - (*root)->left->lHeight;
	(*root)->left->parent = *root;

	(*root)->lHeight = (*root)->left == NULL ? 0 : (*root)->left->height;
	(*root)->height = MAX((*root)->lHeight,(*root)->rHeight) + 1;
	(*root)->diff = (*root)->rHeight - (*root)->lHeight;
	(*root)->parent = parent;
}

void RotateRight(TreeNodePtr *root)
{
	TreeNodePtr tmp = *root,parent = (*root)->parent;
	*root = (*root)->left;
	tmp->left = (*root)->right;
	(*root)->right = tmp;

	(*root)->right->lHeight = (*root)->right->left == NULL ? 0 : (*root)->right->left->height;
	(*root)->right->height = MAX((*root)->right->lHeight,(*root)->right->rHeight) + 1;
	(*root)->right->diff = (*root)->right->rHeight - (*root)->right->lHeight;
	(*root)->right->parent = *root;

	(*root)->rHeight = (*root)->right == NULL ? 0 : (*root)->right->height;
	(*root)->height = MAX((*root)->lHeight,(*root)->rHeight) + 1;
	(*root)->diff = (*root)->rHeight - (*root)->lHeight;
	(*root)->parent = parent;
}

void Rebalance(TreeNodePtr *dest)
{
	if(ABS((*dest)->diff) > 1)//Balance factor violation
	{
		if((*dest)->diff > 0 && (*dest)->right->diff > 0)//RR
		{
			RotateLeft(dest);
		}
		else if((*dest)->diff > 0 && (*dest)->right->diff < 0)//RL
		{
			RotateRight(&((*dest)->right));
			RotateLeft(dest);
		}
		else if((*dest)->diff < 0 && (*dest)->left->diff < 0)//LL
		{
			RotateRight(dest);
		}
		else if((*dest)->diff < 0 && (*dest)->left->diff > 0)//LR
		{
			RotateLeft(&((*dest)->left));
			RotateRight(dest);
		}
	}
}

int Insert(TreeNodePtr *dest,TreeNodePtr node,TreeNodePtr parent)
{
	int ret;
	if(*dest != NULL)
	{
		if(strcmp(node->data,(*dest)->data) < 0)//Insert at left subtree
		{
			ret = Insert(&((*dest)->left),node,*dest);
			if(ret != 0)//Set balance factors only if inserted successfully
			{
				(*dest)->lHeight = (*dest)->left == NULL ? 0 : (*dest)->left->height;
				(*dest)->height = MAX((*dest)->lHeight,(*dest)->rHeight) + 1;
				(*dest)->diff = (*dest)->rHeight - (*dest)->lHeight;

				Rebalance(dest);
			}
			return ret;
		}
		else if(strcmp(node->data,(*dest)->data) > 0)//Insert at right subtree
		{
			ret = Insert(&((*dest)->right),node,*dest);
			if(ret != 0)//Set balance factors only if inserted successfully
			{
				(*dest)->rHeight = (*dest)->right == NULL ? 0 : (*dest)->right->height;
				(*dest)->height = MAX((*dest)->lHeight,(*dest)->rHeight) + 1;
				(*dest)->diff = (*dest)->rHeight - (*dest)->lHeight;

				Rebalance(dest);
			}
			return ret;
		}
		else
		{
			return 0;//Already exists
		}
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
	if((tmp->data = (char*)malloc(strlen(data) + 1)) == NULL) //Memory problem check
		return -1;
	tmp->left = tmp->right = tmp->parent = NULL;
	tmp->lHeight = tmp->rHeight = tmp->diff = 0;
	tmp->height = 1;
	strcpy(tmp->data,data);
	if(!Insert(&(tree->root),tmp,NULL)) //Content already exists
	{
		free(tmp->data);
		free(tmp);
		return 0;
	}
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

int patterncnt(TreeNodePtr node,char *pattern)
{
	int cnt = 0,i = 0,match = 1;
	if(node != NULL)
	{
		cnt += patterncnt(node->left,pattern);
		while(pattern[i] != '\0' && match)
		{
			if(pattern[i] != '*')
			{
				if(pattern[i] != node->data[i])
					match = 0;
				else
					i++;
			}
			else
				i++;
		}
		cnt += match;
		cnt += patterncnt(node->right,pattern);
	}
	return cnt;
}

int AvlTree_NumWordsWithPattern(AvlTreePtr tree,char *pattern)
{
	return patterncnt(tree->root,pattern);
}

int AvlTree_NewSession(AvlTreePtr tree)
{
	if(tree == NULL)//Not yet initialized
		return -1;
	tree->sessions++;
	tree->session = (TreeNodePtr*)realloc(tree->session,tree->sessions * sizeof(TreeNodePtr));
	tree->session[tree->sessions - 1] = NULL;
	return tree->sessions - 1;
}

char *AvlTree_NextWordOfSession(AvlTreePtr tree,int sid)
{
	char *prword;
	if(tree == NULL)//Not yet initialized
		return NULL;
	if(tree->session[sid] == NULL)//First time
	{
		tree->session[sid] = tree->root;
		while(tree->session[sid]->left != NULL)
			tree->session[sid] = tree->session[sid]->left;
	}
	else
	{
		prword = tree->session[sid]->data;
		if(tree->session[sid]->right != NULL)
		{
			tree->session[sid] = tree->session[sid]->right;
			while(tree->session[sid]->left != NULL)
				tree->session[sid] = tree->session[sid]->left;
		}
		else
		{
			tree->session[sid] = tree->session[sid]->parent;
			while(tree->session[sid] != NULL && strcmp(tree->session[sid]->data,prword) < 0)
				tree->session[sid] = tree->session[sid]->parent;
		}
	}
	return tree->session[sid] == NULL ? NULL : tree->session[sid]->data;
}

int AvlTree_DestroyLastSession(AvlTreePtr tree)
{
	if(tree == NULL || tree->sessions == 0)//Error
		return 0;
	tree->sessions--;
	if(tree->sessions == 0)
	{
		free(tree->session);
		tree->session = NULL;
	}
	else
		tree->session = (TreeNodePtr*)realloc(tree->session,tree->sessions);
	return 1;
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