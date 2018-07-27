CC = gcc
FLAGS = -Wall
TARGETS = main.o Board.o AVL_Tree.o

crossword:$(TARGETS)
	$(CC) $(FLAGS) -o crossword $(TARGETS)

main.o: main.c
	$(CC) $(FLAGS) -o main.o -c main.c

Board.o: Board.c
	$(CC) $(FLAGS) -o Board.o -c Board.c

AVL_Tree.o:AVL_Tree.c
	$(CC) $(FLAGS) -o AVL_Tree.o -c AVL_Tree.c

.PHONY : clean

clean:
	rm -f $(TARGETS) crossword