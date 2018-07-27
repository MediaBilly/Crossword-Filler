#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Board.h"
#include "AVL_Tree.h"

#define MAX_LEN 30

typedef struct WordTree
{
	AvlTreePtr tree;
	int enabled;
} WordTree;

struct Board
{
	char **table;
	WordTree wordTree[MAX_LEN];
	int dim;
};

int Board_Initialize(Boardptr *board,const char* boardFile,const char* dictFile)
{
	FILE *file;
	int i,j,lenh = 0,lenv = 0,len;
	char buf[MAX_LEN+1];
	//Allocate memory board struct
	if(((*board = (Boardptr)malloc(sizeof(struct Board)))) == NULL)
	{
		printf("Not enough memory.\n");
		return 0;
	}
	//Read Board file and initialize board struct
	for(i = 0;i < MAX_LEN;i++)
	{
		(*board)->wordTree[i].tree = NULL;
		(*board)->wordTree[i].enabled = 0;
	}
	if((file = fopen(boardFile,"r")) == NULL)
	{
		printf("Board file does not exist.\n");
		return 0;
	}
	fscanf(file, "%d", &((*board)->dim));
	if(((*board)->table = (char**)malloc(((*board)->dim + 2) * sizeof(char*))) == NULL)
	{
		printf("Not enough memory.\n");
		return 0;
	}
	for(i = 0;i < (*board)->dim + 2;i++)
	{
		if(((*board)->table[i] = (char*)malloc(((*board)->dim + 2) * sizeof(char))) == NULL)
		{
			printf("Not enough memory.\n");
		    return 0;
		}
		(*board)->table[i][0] = (*board)->table[i][(*board)->dim + 1] = '#';
		for(j = 1;j <= (*board)->dim;j++)
			(*board)->table[i][j] = ((i == 0 || i == (*board)->dim + 1) ? '#' : ' ');
	}

	while(fscanf(file, " %d %d", &i,&j) != EOF)
		(*board)->table[i][j] = '#';

    fclose(file);

    //Scan board to enable the available lengths
    for(i = 1;i < (*board)->dim + 2;i++)
    {
    	for(j = 1;j < (*board)->dim + 2;j++)
    	{
    		//Horizontal check
    		if((*board)->table[i][j] == ' ')
    			lenh++;
    		else
    		{
    			if(lenh != 0)
    			{
    				(*board)->wordTree[lenh - 1].enabled = 1;
    				lenh = 0;
    			}
    		}
    		//Vertical check
    		if((*board)->table[j][i] == ' ')
    			lenv++;
    		else
    		{
    			if(lenv != 0)
    			{
    				(*board)->wordTree[lenv - 1].enabled = 1;
    				lenv = 0;
    			}
    		}
    	}
    }

    /*printf("Available Lengths:");
    for(i = 0;i < MAX_LEN;i++)
    	if((*board)->wordTree[i].enabled)
    		printf("%d ",i + 1);
    printf("\n");*/

    //Read Words
    if((file = fopen(dictFile,"r")) == NULL)
	{
		printf("Dictionary file does not exist.\n");
		return 0;
	}
	while(fscanf(file, "%s",buf) != EOF)
	{
		len = strlen(buf);
		if((*board)->wordTree[len - 1].enabled)
		{
			if((*board)->wordTree[len - 1].tree == NULL)//Not yet initialized
			{
				if(!AvlTree_Create(&((*board)->wordTree[len - 1].tree)))//Out of memory
				{
					printf("Not enough memory.\n");
					fclose(file);
					return 0;
				}
				else
				{
					if(AvlTree_Insert((*board)->wordTree[len - 1].tree,buf) == -1)//Out of memory
					{
						printf("Not enough memory.\n");
						fclose(file);
						return 0;
					}
				}
			}
			else
			{
				if(AvlTree_Insert((*board)->wordTree[len - 1].tree,buf) == -1)
				{
					printf("Not enough memory.\n");
					fclose(file);
					return 0;
				}
			}
		}
	}
	fclose(file);

	/*printf("%d\n",AvlTree_NumWordsWithPattern((*board)->wordTree[4].tree,"te**s"));
	int s = AvlTree_NewSession((*board)->wordTree[2].tree);
	char *w;
	if(s != -1)
	{
		while((w = AvlTree_NextWordOfSession((*board)->wordTree[2].tree,s)) != NULL)
			printf("%s\n",w);
		AvlTree_DestroyLastSession((*board)->wordTree[2].tree);
	}*/
	return 1;
}

void Board_Print(Boardptr board)
{
	int i,j;
	for(i = 0;i < board->dim + 2;i++)
	{
		for(j = 0;j < board->dim + 2;j++)
			printf(board->table[i][j] == '#' ? "###" : " %c ",board->table[i][j]);
		printf("\n");
	}
}

int Board_Destroy(Boardptr *board)
{
	int i;
	if(*board == NULL)//Not yet initialized
		return 0;
	for(i = 0;i < (*board)->dim + 2;i++)
		free((*board)->table[i]);
	free((*board)->table);
	for(i = 0;i < MAX_LEN;i++)
		if((*board)->wordTree[i].tree != NULL)
			AvlTree_Destroy(&((*board)->wordTree[i].tree));
	free(*board);
	return 1;
}