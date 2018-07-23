#include <stdio.h>
#include "AVL_Tree.h"

int main(int argc, char const *argv[])
{
	AvlTreePtr tree = NULL;
	FILE *dict = fopen(argv[1], "r");
	char buf[80];
	int r;
	if(AvlTree_Create(&tree))
		printf("Tree created successfully.\n");
	else
		printf("Not enough memory.\n");

	while(fscanf(dict, "%s", buf) != EOF)
	{
		r = AvlTree_Insert(tree,buf);
		if(r == 0)
			printf("Word %s already exists\n",buf);
		else if(r == -1)
		{
			printf("Not enough memory.\n");
			break;
		}
	}

	printf("%d words inserted.\n", AvlTree_NumWords(tree));

	fclose(dict);

	if(AvlTree_Destroy(&tree))
		printf("Tree destroyed successfully.\n");
	else
		printf("Tree not yet initialized.\n");
	return 0;
}