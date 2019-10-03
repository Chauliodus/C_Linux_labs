/* myqsort.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#define USAGE "input count of strings and the strings themself"

int cmpstrp(const void *p1, const void *p2)
{
	return strcmp(*(char ** const)p1, *(char ** const)p2);
}

int main(int argc, char ** argv)
{
	if(argc < 2)
	{
		perror(USAGE);
		exit(EXIT_FAILURE);
	}
	
	int STRINGSCOUNT = atoi(argv[1]);
	
	// allocate memory for an array of pointers 
	char ** strings = (char**)malloc(sizeof(char*)*STRINGSCOUNT);
	
	size_t i;
	
	printf("\nPURE ARRAY:\n\n");
	
	// initialize strings
	for(i = 0; i < STRINGSCOUNT; i++)
	{
		strings[i] = (char*)malloc(sizeof(char)*strlen(argv[i + 2]));
		strcpy(strings[i], argv[i + 2]);
		printf("%s\n", strings[i]);
	}
	
	qsort(strings, STRINGSCOUNT, sizeof(char*), cmpstrp);
	
	// show the result
	printf("\nSORTED ARRAY:\n\n");
	
	for(i = 0; i < STRINGSCOUNT; i++)
		printf("%s\n", strings[i]);
		
	printf("\n");
	
	free(strings);
	
	return 0;
}
