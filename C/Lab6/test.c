/* test.c */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

extern int num;

int main(int argc, char* argv[])
{		
	srand(time(0));
	num = 50;
	pid_t mypid = getpid();
    pid_t pid1 = fork();	
    pid_t pid2;
    pid_t pid3;
    if(getpid() == mypid)
		pid2 = fork();
	if(getpid() == mypid)
		pid3 = fork();
		
	if(pid1 == 0 || pid2 == 0 || pid3 == 0)
	{
		printf("pid1 = %d pid2 = %d pid3 = %d\n", pid1, pid2, pid3);
		printf("pid = %d num = %d\n", getpid(), num);
		num+=rand()%50;
		printf("pid = %d num = %d\n", getpid(), num);
		exit(getpid());
	}
	
	if(getpid() == mypid)
	{
		wait(&pid1);
		wait(&pid2);
		wait(&pid3);
		printf("num = %d\n", num);
		exit(0);
	}	
}
