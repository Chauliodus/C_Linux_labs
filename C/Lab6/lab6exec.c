/* lab6exec.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>

main()
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
