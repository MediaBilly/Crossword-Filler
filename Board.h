#ifndef _BOARD_
#define _BOARD_

typedef struct Board *Boardptr;

int Board_Initialize(Boardptr *board,const char* boardFile,const char* dictFile);
void Board_Print(Boardptr board);
int Board_Destroy(Boardptr *board);
#endif