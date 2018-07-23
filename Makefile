CC = gcc
FLAGS = -Wall
TARGETS = main.o AVL_Tree.o

crossword:$(TARGETS)
	$(CC) $(FLAGS) -o crossword $(TARGETS)

main.o: main.c
	$(CC) $(FLAGS) -o main.o -c main.c

AVL_Tree.o:AVL_Tree.c
	$(CC) $(FLAGS) -o AVL_Tree.o -c AVL_Tree.c