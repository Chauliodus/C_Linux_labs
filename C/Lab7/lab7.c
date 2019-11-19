/* lab7.c */
/* Written by Alexey Shved for EltexProgrammingSchool, 17 oct 2019.
 * 
 * Program imitates survival horror about hungry bear, who is eating 
 * honey all the time, and bees, forced to provide a honey for him.
 * 
 * Connect between bear and bees performed by pipes.
 * */
 
 #define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#define MAX_LEN 10
#define USAGE "./lab7 <bees count> <Pooh hunger (how much he eats)> <Pooh survivability (how long he can stand alive)>\n"

void blockfile(int);

void unblockfile(int);

int gethoney(int * fd, int log)
{
	close(fd[0]);
	srand(getpid());
	char * honey = "1 honey";
	char buf[80] = { 0 };
	size_t sleeptime;
	do
	{
		sleeptime = rand()%5 + 1;
		memset(buf, 0, 80);
		sprintf(buf, 
				"-- bee #%d carries a piece of honey (%d sec)...\n", 
				getpid(), 
				sleeptime);
		printf("%s", buf);
		blockfile(log);
		write(log, buf, sizeof(char) * strlen(buf));
		unblockfile(log);
		usleep(sleeptime);
		lseek(fd[1], 0, 0);
		lockf(fd[1], F_LOCK, MAX_LEN);
		write(fd[1], honey, sizeof(honey));
		lseek(fd[1], 0, 0);
		lockf(fd[1], F_ULOCK, MAX_LEN);
		memset(buf, 0, 80);
		sprintf(buf, "пчела #%d вернулась в улей\n", getpid());
		printf("%s", buf);
		blockfile(log);
		write(log, buf, sizeof(char) * strlen(buf));
		unblockfile(log);
				
	} while(1);
	return 0;
}

//----------------------------------------------------------------------

void blockfile(int fd)
{
	lseek(fd, 0, 0);
	lockf(fd, F_LOCK, 80);
}

void unblockfile(int fd)
{
	lseek(fd, 0, 0);
	lockf(fd, F_ULOCK, 80);
}

//----------------------------------------------------------------------

int main(int argc, char ** argv)
{
	// if too view arguments
	if(argc < 4)
	{
		perror(USAGE);
		exit(0);
	}
	
	// initialization of variables
	const size_t beecount = (size_t)atoi(argv[1]);
	const size_t poohhunger = (size_t)atoi(argv[2]);
	const size_t survivability = (size_t)atoi(argv[3]);
	
	// fd transports data about honey amount from children
	int fd[beecount][2], i; 
	for(i = 0; i < beecount; i++)
		if(pipe2(fd[i], O_DIRECT) == -1)
		{
			perror("pipe failed\n");
			exit(1);
		}
	
	// memory allocation	
	pid_t *pid;
	pid = (pid_t *)calloc(beecount, sizeof(pid_t));
	
	int log;
	mode_t logmode = S_IRWXU | S_IRWXG | S_IRWXO;
	log = open("LOG", O_CREAT | O_RDWR | O_APPEND | O_TRUNC, logmode);
	if(log <= 0)
	{
		perror("parent: open log");
		exit(0);
	}
	
	// "Hive": let's create bees (children)!
	char buff[80] = { 0 };
	for(i = 0; i < beecount; i++)
	{
		pid[i] = fork();
		if (pid[i] == 0)
		{
			memset(buff, 0, 80);
			sprintf(buff, "Bee%d flied out of hive (pid = %d)\n%c", 
														   i + 1, 
														   getpid(), 
														   (char)0);
			printf("%s", buff);
			blockfile(log);
			write(log, buff, sizeof(char) * strlen(buff));
			unblockfile(log);
			gethoney(fd[i], log);
			exit(0);
		}
	}
	
	// Pooh is a process that is only decreasing value in file "barrel"
	pid_t pooh;
	int barrel;
	mode_t mode = S_IRUSR | S_IWUSR;
	barrel = open("barrel", O_TRUNC | O_RDWR | O_CREAT, mode);
	
	if(barrel == -1)
	{
		perror("barrel failed");
		exit(1);
	}
	
	pooh = fork();
	if(pooh == 0)
	{
		char sur[5] = { 0 };
		char hun[5] = { 0 };
		sprintf(sur, "%d", survivability);
		sprintf(hun, "%d", poohhunger);
		execl("pooh", "pooh", sur, hun, NULL);
	}

	for(i = 0; i < beecount; i++)
		close(fd[i][1]);
	
	char buf[MAX_LEN] = { 0 };
	
	int pooh_exit_status;
	
	while(1) // loop gets honey from buf, puts it into a barrel
	{
		waitpid(pooh, &pooh_exit_status, WNOHANG);
		
		if(WIFEXITED(pooh_exit_status))
		{	
			for(i = 0; i < beecount; i++)
			{
				if(kill(pid[i], SIGINT) == 0)
				{
					memset(buff, 0, 80);
					sprintf(buff, "}i{ Bee%d killed (pid = %d)\n", i, 
																pid[i]);
					printf("%s", buff);
					blockfile(log);
					write(log, buff, sizeof(char) * strlen(buff));
					unblockfile(log);
				}
				else 
				{
					perror("sigint");
					exit(0);
				}
			}
			break;
		}
		
		// honey is ready to putting into barrel
		int ready_honey = 0;
		
		// honey got from bees
		for(i = 0; i < beecount; i++)
		{
			blockfile(fd[i][0]);
			read(fd[i][0], buf, MAX_LEN);
			unblockfile(fd[i][0]);
			
			ready_honey += atoi(buf);
			memset(buff, 0, 80);
			sprintf(buff, "...bee brought %d honey\n", ready_honey);
			printf("%s", buff);
			blockfile(log);
			write(log, buff, sizeof(char) * strlen(buff));
			unblockfile(log);
			
			// count honey in the barrel
			blockfile(barrel);
			memset(buf, 0, 80);
			read(barrel, buf, 10);
			if(atoi(buf) == 0)
			{
				memset(buff, 0, 80);
				sprintf(buff, "I see no honey in barrel\n");
				printf("%s", buff);
				blockfile(log);
				write(log, buff, sizeof(char) * strlen(buff));
				unblockfile(log);
			}
			else
			{
				memset(buff, 0, 80);
				sprintf(buff, "I see in barrel %s honey\n", buf);
				printf("%s", buff);
				blockfile(log);
				write(log, buff, sizeof(char) * strlen(buff));
				unblockfile(log);
			}
			ready_honey += atoi(buf);
		
			memset(buff, 0, 80);
			sprintf(buf, "%d", ready_honey);
			sprintf(buff, 
					"I add honey into the barrel. Now there: %s\n", 
					buf);
			printf("%s", buff);
			blockfile(log);
			write(log, buff, sizeof(char) * strlen(buff));
			unblockfile(log);
			
			lseek(barrel, 0, 0);
			write(barrel, buf, sizeof(char) * strlen(buf));
			unblockfile(barrel);
			
			ready_honey = 0;
		}
	}
	
	memset(buff, 0, 80);
	sprintf(buff, "Bear ate %d honey\n", WEXITSTATUS(pooh_exit_status));
	printf("%s", buff);
	blockfile(log);
	write(log, buff, sizeof(char) * strlen(buff));
	unblockfile(log);
	
	for(i = 0; i < beecount; i++)
	{
		free(&pid[i]);
		close(fd[i][0]);
	}
	//free(pid);
	close(barrel);
	close(log);

	return 0;
}
