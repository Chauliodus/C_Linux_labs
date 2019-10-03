/* lab3-3.c */

/* This program contains a struct that describes monsters. 
* Monster has a name and a level, that generates automatically
* (random). User can sort the monsters using quick sort.
* Written by Alexey Shved for Eltex Programming School, 28 
* sept 2019.
* */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct monster 
{
	char* name;
	size_t level;
};

// compares monsters by name length
int cmpName(const void * p1, const void * p2)
{
	struct monster * m1 = *(struct monster **)p1;
	struct monster * m2 = *(struct monster **)p2;
	return strlen(m2->name) - strlen(m1->name);
}

int createMonster(struct monster * mon)
{
	mon->name = (char*)malloc(sizeof(char)*50);
	printf("Its name?\n");
	scanf("%s", mon->name);
	mon->level = rand()%100 + 1;
	printf("Its level is %d.\n", mon->level);
	printf("Monster %s created\n\n", mon->name);
	return 0;
}

int main(int argc, char ** argv)
{
	srand(time(NULL));
	printf("input count of monsters: \n");
	size_t count;
	scanf("%d", &count);
	
	// allocate the memory for an array of structs
	struct monster ** monsters = 
		(struct monster**)malloc(sizeof(struct monster*)*count);
	
	// allocate memory for each monster in array, create monsters
	size_t i;
	for(i = 0; i < count; i++)
	{
		monsters[i] = (struct monster*)malloc(sizeof(struct monster));
		if(createMonster(monsters[i]) != 0)
			printf("Something went wrong\n");
	}
	
	qsort(monsters, count, sizeof(struct monster *), cmpName);
	
	for(i = 0; i < count; i++)
	{
		printf("Mon%d name = %s\n", i, monsters[i]->name);
		free(monsters[i]);
	}

	free(monsters);
	
	return 0;
}
