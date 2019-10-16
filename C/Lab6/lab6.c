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
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
 
#define USAGE "./lab6 <count of units> <dungeon reachness>"
 
int getGold()
{
	int gold, fd, step = 0;
	char buf[10] = { 0 };
	srand(getpid());
	do
	{
		fd = open("mine.txt", O_RDWR);
		lseek(fd, 0, 0);
		lockf(fd, F_LOCK, sizeof(char) * 4);
		read(fd, buf, sizeof(char) * 4);
		printf("%d %s\n", getpid(), buf);
		gold = atoi(buf);
		if(gold > 0)
		{
			gold--;
			lseek(fd, 0, 0);
			sprintf(buf, "%d", gold);
			write(fd, buf, 8);
		}
		step++;
		lseek(fd, 0, 0);
		lockf(fd, F_ULOCK, 8);
		close(fd);
		sleep(rand()%4);
	}while(gold > 0);
	printf("There is no gold anymore; I did %d trips\n", step);
	return step;
}
 
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
			{
				int c = getGold();
				exit(c);
			}
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
