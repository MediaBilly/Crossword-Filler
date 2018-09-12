#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "Board.h"
#include "AVL_Tree.h"

#define MAX_LEN 30

typedef struct MoveList *MoveListPtr;

typedef struct Move
{
	int hor;
	int row;
	int col;
	int ecol;
	int length;
	int cr_hor;
	int cr_row;
	int cr_col;
	int cr_ecol;
	int firstmove;
	int *cp;
} Move;

struct MoveList
{
	Move moveData;
	MoveListPtr next;
};

typedef struct WordTree
{
	AvlTreePtr tree;
	int enabled;
} WordTree;

struct Board
{
	char **table;
	WordTree wordTree[MAX_LEN];
	MoveListPtr movesList;
	int dim;
	Move angryMove;
};

int MoveList_AddMove(MoveListPtr *list,Move moveData)
{
	MoveListPtr tmp;
	if(*list == NULL)//First time
	{
		if((*list = (MoveListPtr)malloc(sizeof(struct MoveList))) == NULL)
			return 0;
		(*list)->moveData = moveData;
		(*list)->next = NULL;
	}
	else
	{
		tmp = *list;
		if((*list = (MoveListPtr)malloc(sizeof(struct MoveList))) == NULL)
			return 0;
		(*list)->moveData = moveData;
		(*list)->next = tmp;
	}
	return 1;
}

int MoveList_RemoveLastMove(MoveListPtr *list)
{
	MoveListPtr toDel;
	if(*list == NULL)
		return 0;
	toDel = *list;
	*list = (*list)->next;
	if(toDel->moveData.cp != NULL)
		free(toDel->moveData.cp);
	free(toDel);
	return 1;
}

void MoveList_Destroy(MoveListPtr *list)
{
	MoveListPtr toDel;
	while(*list != NULL)
	{
		toDel = *list;
		*list = (*list)->next;
		free(toDel->moveData.cp);
		free(toDel);
	}
}

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
	(*board)->movesList = NULL;
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
	return 1;
}

Move getNextMove(Boardptr board)
{
	int i,j,min,minlength,len = 0,found = 0,p,num,gaps = 0;
	Move move,prmove;
	char buf[MAX_LEN + 1];
	MoveListPtr movesList = board->movesList;
	move.hor = movesList == NULL ? 1 : !movesList->moveData.hor;
	move.firstmove = movesList == NULL ? 1 : 0;
	if(movesList == NULL)//Firsttime
	{
		//Find the wordlist with the least amount of words
		min = INT_MAX;
		for(i = 1;i < MAX_LEN;i++)
			if(board->wordTree[i].enabled && board->wordTree[i].tree != NULL && AvlTree_NumWords(board->wordTree[i].tree) < min)
			{
				min = AvlTree_NumWords(board->wordTree[i].tree);
				minlength = i + 1;
			}

		move.length = minlength;
		for(i = 1;i < board->dim + 2 && !found;i++)
		{
			p = 1;
			for(j = 1;j < board->dim + 2 && !found;j++)
			{
				if(board->table[move.hor ? i : j][move.hor ? j : i] != '#')
					len++;
				else
				{
					if(len == minlength)
					{
						found = 1;
						move.row = i;
						move.col = p;
						move.ecol = j - 1;
						move.cr_hor = move.cr_row = move.cr_col = move.cr_ecol = -1;
					}
					p = j + 1;
					len = 0;
				}
			}
		}
		//If not horizontal try vertical
		if(!found)
		{
			move.hor = 0;
			for(i = 1;i < board->dim + 2 && !found;i++)
			{
				p = 1;
				for(j = 1;j < board->dim + 2 && !found;j++)
				{
					if(board->table[move.hor ? i : j][move.hor ? j : i] != '#')
						len++;
					else
					{
						if(len == minlength)
						{
							found = 1;
							move.row = i;
							move.col = p;
							move.ecol = j - 1;
							move.cr_hor = move.cr_row = move.cr_col = move.cr_ecol = -1;
						}
						p = j + 1;
						len = 0;
					}
				}
			}
		}
	}
	else
	{
		min = INT_MAX;
		while(movesList != NULL)
		{
			prmove = movesList->moveData;
			for(j = prmove.col;j <= prmove.ecol;j++)
			{
				i = prmove.row;
				gaps = 0;
				while(board->table[prmove.hor ? i : j][prmove.hor ? j : i] != '#')
					i--;
				p = ++i;
				while(board->table[prmove.hor ? p : j][prmove.hor ? j : p] != '#')
				{
					if(board->table[prmove.hor ? p : j][prmove.hor ? j : p] == ' ')
					{
						gaps++;
						buf[p - i] = '*';
					}
					else
						buf[p - i] = board->table[prmove.hor ? p : j][prmove.hor ? j : p];
					p++;
				}
				buf[p - i] = '\0';
				len = strlen(buf);
				if(gaps != 0 && len > 1)
				{
					num = AvlTree_NumWordsWithPattern(board->wordTree[len - 1].tree,buf);
					if(num == 0)
					{
						found = 0;
						break;
					}
					else
					{
						found = 1;
						if(num < min)
						{
							min = num;
							move.row = j;
							move.col = i;
							move.ecol = p - 1;
							move.length = len;
							move.hor = !prmove.hor;
							move.cr_hor = prmove.hor;
							move.cr_row = prmove.row;
							move.cr_col = prmove.col;
							move.cr_ecol = prmove.ecol;
							if(movesList->next != NULL && movesList->next->next == NULL)
								move.firstmove = 1;
						}
					}
				}
			}
			movesList = movesList->next;
			if(movesList != NULL && movesList->moveData.firstmove)
				break;
		}
		if(!found)//If we dont a find a move that crosses a previous one
		{
			printf("bl\n");
			//Find the move with the least options of words
			min = INT_MAX;
			do
			{
				for(i = 1;i < board->dim + 2;i++)
				{
					p = 1;
					for(j = 1;j < board->dim + 2;j++)
					{
						if(board->table[move.hor ? i : j][move.hor ? j : i] != '#')
						{
							len++;
							if(board->table[move.hor ? i : j][move.hor ? j : i] == ' ')
							{
								gaps++;
								buf[j - p] = '*';
							}
							else
								buf[j - p] = board->table[move.hor ? i : j][move.hor ? j : i];
						}
						else
						{
							buf[j - p] = '\0';
							if(gaps > 0 && len > 1)
							{
								num = AvlTree_NumWordsWithPattern(board->wordTree[len - 1].tree, buf);
								if(num > 0 && num < min)
								{
									min = num;
									found = 1;
									move.row = i;
									move.col = p;
									move.ecol = j - 1;
									move.length = len;
									move.cr_hor = move.cr_row = move.cr_col = move.cr_ecol = -1;
								}
							}
							p = j + 1;
							len = gaps = 0;
						}
					}
				}
				if(!found)
				{
					printf("blah\n");
					move.hor = !move.hor;
				}
			}
			while(!found);
		}
	}
	if(found)
		move.cp = (int*)calloc(move.length,sizeof(int));
	else
		move = (Move){ -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , NULL };
	return move;
}

int placeWord(Boardptr board,char *word,Move move)
{
	int i,j;
	for(i = move.col;i <= move.ecol;i++)
	{
		if(board->table[move.hor ? move.row : i][move.hor ? i : move.row] != ' ')
		{
			if(board->table[move.hor ? move.row : i][move.hor ? i : move.row] != word[i - move.col])
			{
				for(j = move.col;j < i;j++)
					if(!move.cp[j - move.col])
						board->table[move.hor ? move.row : j][move.hor ? j : move.row] = ' ';
				return 0;
			}
			else
				move.cp[i - move.col] = 1;
		}
		else
			board->table[move.hor ? move.row : i][move.hor ? i : move.row] = word[i - move.col];
	}
	return 1;
}

void removeLastWord(Boardptr board)
{
	Move move = board->movesList->moveData;
	int i;
	for(i = move.col;i <= move.ecol;i++)
		if(!move.cp[i - move.col])
			board->table[move.hor ? move.row : i][move.hor ? i : move.row] = ' ';
}

int checkMove(Boardptr board,Move move)
{
	int i,j,p,gaps;
	char buf[MAX_LEN + 1];
	for(j = move.col;j <= move.ecol;j++)
	{
		i = move.row;
		gaps = 0;
		while(board->table[move.hor ? i : j][move.hor ? j : i] != '#')
			i--;
		p = ++i;
		while(board->table[move.hor ? p : j][move.hor ? j : p] != '#')
		{
			if(board->table[move.hor ? p : j][move.hor ? j : p] == ' ')
			{
				buf[p - i] = '*';
				gaps++;
			}
			else
				buf[p - i] = board->table[move.hor ? p : j][move.hor ? j : p];
			p++;
		}
		buf[p - i] = '\0';
		if(strlen(buf) > 1)
		{
			if(gaps != 0)
			{
				if(!AvlTree_PatternExists(board->wordTree[strlen(buf) - 1].tree,buf))
					return 0;
			}
			else
			{
				if(!AvlTree_Search(board->wordTree[strlen(buf) - 1].tree,buf))
					return 0;
			}
		}
	}
	return 1;
}

int filled(Boardptr board)
{
	int i,j;
	for(i = 1;i < board->dim + 1;i++)
		for(j = 1;j < board->dim + 1;j++)
			if(board->table[i][j] == ' ')
				return 0;
	return 1;
}

int Board_Solve(Boardptr board)
{
	if(filled(board))
		return 1;
	Move move = getNextMove(board);
	if(move.row == -1)
		return 0;
	if(!(MoveList_AddMove(&(board->movesList),move)))
		return 0;
	int session;
	char *w;
	if((session = AvlTree_NewSession(board->wordTree[move.length - 1].tree)) == -1)
		return 0;
	//printf("(%d,%d) -(%d,%d) : %d\n",move.row,move.col,move.row,move.ecol,move.hor);
	//Board_Print(board);
	while((w = AvlTree_NextWordOfSession(board->wordTree[move.length - 1].tree,session)) != NULL)
	{
		if(placeWord(board,w,move))
		{
			if(checkMove(board,move))
			{
				if(Board_Solve(board))
					return 1;
				else
				{
					removeLastWord(board);
					if(board->angryMove.cr_hor != -1)
						if(board->angryMove.cr_hor != move.hor || board->angryMove.cr_row != move.row || board->angryMove.cr_col != move.col || board->angryMove.cr_ecol != move.ecol)
						{
							//printf("(%d,%d) -(%d,%d) : %d\n",board->angryMove.cr_row,board->angryMove.cr_col,board->angryMove.cr_row,board->angryMove.cr_ecol,board->angryMove.cr_hor);
							MoveList_RemoveLastMove(&(board->movesList));
							AvlTree_EndSession(board->wordTree[move.length - 1].tree,session);
							return 0;
						}
				}
			}
			else
				removeLastWord(board);
		}
	}
	MoveList_RemoveLastMove(&(board->movesList));
	board->angryMove = move;
	return 0;
}

void Board_Print(Boardptr board)
{
	int i,j;
	//system("clear");
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
	MoveList_Destroy(&((*board)->movesList));
	free(*board);
	return 1;
}