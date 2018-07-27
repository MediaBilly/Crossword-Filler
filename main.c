#include <stdio.h>
#include "Board.h"

int main(int argc, char const *argv[])
{
	Boardptr board = NULL;

	if(!Board_Initialize(&board,argv[1],argv[2]))
		return 0;

	Board_Print(board);

	if(Board_Destroy(&board))
		printf("Board destroyed successfully.\n");
	else
		printf("Board not yet initialized.\n");
	
	return 0;
}