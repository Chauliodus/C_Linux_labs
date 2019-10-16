/* lab6-1.c */
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
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
 
#define USAGE "./lab6 <count of units> <dungeon reachness>"
 
int main(int argc, char ** argv)
{
	if(argc < 3)
	{
		perror(USAGE);
		exit(1);
	}
	
	// инициализация необходимых переменных
	size_t unitsCount = (size_t)atoi(argv[1]);
	size_t gold = (size_t)atoi(argv[2]);
	
	pid_t * pids;
	pids = calloc(unitsCount, sizeof(pid_t));

	int stat;
	
	int fd;
	fd = open("mine.txt", O_RDWR | O_EXCL);
	if(fd < 0)
	{
		perror("fd failed\n");
		exit(1);
	}
	lseek(fd, 0, 0); // SEEK_SET - from begin, SEEK_CUR - current pos, SEEK_END - from end, 
					 //equal to 0. 1 and 2 respectively  
	uint8_t sgold[10] = { 0 };
	sprintf((char *)sgold, "%d", gold);
	write(fd, (char *)sgold, strlen((char *)sgold) * sizeof(char));
	close(fd);
	
	char buf[10] = { 0 }; 
	
	size_t i;
	for (i = 0; i < unitsCount; i++)
	{
		pids[i] = fork();
		if(pids[i] == 0)
				execl("./lab6exec", "lab6exec", NULL);
	}

	int * result;
	result = calloc(unitsCount, sizeof(int));
	
	fd = open("mine.txt", O_RDONLY);
	read(fd, buf, 8);
	if(atoi(buf) > 0)
	{
		printf("I will sleep for %d mikrosec\n", atoi(buf));
		sleep(atoi(buf));
	}
		
	for(i = 0; i < unitsCount; i++)
	{
		waitpid(pids[i], &stat, 0);
		result[i] = WEXITSTATUS(stat);
		printf("%d did %d trips\n", pids[i], result[i]);
	}
	
	printf("all were gone\n");
	
	return 0;
}
