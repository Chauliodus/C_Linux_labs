/* test2.c */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

#define COUNT 5
int main(int argc, char* argv[])
{		
        pid_t pid[COUNT];	/* fork returns type pid_t */
        srand(time(NULL));
        size_t i;
        for(i = 0; i < COUNT; i++)
        {
			pid[i] = fork();
			printf("pid[%d] = %d, getpid() = %d\n", i, pid[i], getpid());
		}
        printf("My pid=%d, my ppid = %d \n", getpid(), getppid());
}
