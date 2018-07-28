#include <stdio.h>
#include "Board.h"

int main(int argc, char const *argv[])
{
	Boardptr board = NULL;

	if(!Board_Initialize(&board,argv[1],argv[2]))
		return 0;

	if(Board_Solve(board))
	{
		printf("Puzzle Solved!\n");
		Board_Print(board);
	}
	else
		printf("No solution.\n");


	if(!Board_Destroy(&board))
		printf("Board not yet initialized.\n");
	
	return 0;
}