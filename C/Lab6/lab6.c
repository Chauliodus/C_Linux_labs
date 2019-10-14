/* lab6.c */
/* Written by Alexey Shved for EltexProgrammingSchool, 12 oct. 2019
 * "Warcraft"
 * Units get gold from the dungeon, delaying for random time, while the
 * dungeon isn't empty.
 * */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>
 
#define USAGE "./lab6 <count of units> <dungeon reachness>"
 
void getGold(size_t * dungeonReachness)
{
	*dungeonReachness = *dungeonReachness - 1;
	sleep(rand()%20);
}
 
int main(int argc, char ** argv)
{
	// чтобы рандомное число было разным при каждом запуске
	srand(time(NULL));
	
	if(argc < 3)
	{
		perror(USAGE);
		exit(1);
	}

	// инициализация необходимых переменных
	size_t unitsCount = (size_t)atoi(argv[1]);
	size_t dungeonReachness = (size_t)atoi(argv[2]);

	// добыча золота
	size_t i;
	pid_t pid[unitsCount];
	for(i = 0; i < unitsCount; i++)
	{
		pid[i] = fork();
		if(pid[i] == 0) // if proc has no children... it is a child itself!
		{
			while(dungeonReachness > 0)
			{
				getGold(&dungeonReachness);
			}
			exit(0);
		}
		printf("Unit #%d got some gold. There are %d gold in dungeon.\n",
			pid[i], (int)dungeonReachness);
	}
	
	while(wait(NULL))
		continue;

	return 0;
}
