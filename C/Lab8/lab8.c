/* lab8.c */
/* Message queue */
/* Made by Alex Shved for EPS, 18 Nov 2019 */

/* Шарики. 
 * 
 * Координаты заданного количества шариков изменяются на случайную 
 * величину по вертикали и горизонтали. 
 * 
 * При выпадении шарика за нижнюю границу допустимой области шарик 
 * исчезает. 
 * 
 * Изменение координат каждого шарика в отдельном процессе (потоке).*/

#include <stdio.h>
#include <sys/msg.h> 
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/errno.h>

#define USAGE "./lab8 balls_count"
#define BASE 10
#define DOWNBOUND 5
#define UPBOUND 15

struct msqbuf_parent
{
	long type;
	int xvalue, yvalue;
};

struct msqbuf_child
{
	long type;
	pid_t pid;
	int x, y, steps;
};

int getqueue()
{
	key_t key;
	int msqid;
	struct msqid_ds info;
	do {
		key = ftok("/etc/sysctl.conf", 'm');
		if(key == -1) {
			perror("child key");
			exit(1);
		}
		msqid = msgget(key, IPC_CREAT | 0660);
		if(msqid == -1)
		{
			perror("msgget");
			exit(1);
		}
		msgctl(msqid, IPC_STAT, &info);
		if(info.__msg_cbytes > 0){
			printf("Queue %d is not clear\n", msqid);
			msgctl(msqid, IPC_RMID, NULL);
		}
	} while(info.__msg_cbytes > 0);
	printf("msqid = %d is OK\n", msqid);
	return msqid;
}

int fchild(int mynum, int msqid)
{
	srand(getpid());
	struct msqbuf_parent parentbuf;
	struct msqbuf_child childbuf;
	childbuf.type = getpid();
	childbuf.pid = getpid();
	childbuf.steps = 0;
	childbuf.x = BASE;
	childbuf.y = BASE;
	printf("Ball %d was thrown from %d;%d\n", mynum, childbuf.x, 
			childbuf.y);
	int n;
	while(1) {
		n = msgrcv(msqid, &parentbuf, sizeof(struct msqbuf_parent) - 
					sizeof(long), mynum, 0);
		if (n == sizeof(struct msqbuf_parent) - sizeof(long)) {
			childbuf.x += parentbuf.xvalue;
			childbuf.y += parentbuf.yvalue;
			childbuf.steps++;
			printf("Child %d (%d) : x %d, y %d\n", mynum, getpid(), 
					childbuf.x, childbuf.y);
		}
		msgsnd(msqid, &childbuf, sizeof(struct msqbuf_child) - 
				sizeof(long), 0);
	}
	exit(childbuf.steps);
}

inline int outofbounds(const struct msqbuf_child * childbuf)
{
	if((childbuf->x <= DOWNBOUND) || (childbuf->x >= UPBOUND) 
		|| (childbuf->y <= DOWNBOUND) || (childbuf->y >= UPBOUND))
		return 1;
	else return 0;
}
		
void fparent(int msqid, pid_t * pid, int count)
{
	struct msqbuf_parent parentbuf;
	struct msqbuf_child * childbuf;
	childbuf = (struct msqbuf_child *)malloc(count * sizeof(struct 
				msqbuf_child));
	int all = count, i, n;
	while(1) {
		sleep(1);
		for(i = 0; i < count; i++){
			parentbuf.xvalue = rand()%6 - 3;
			parentbuf.yvalue = rand()%6 - 3;
			parentbuf.type = i + 1;
			printf("Parent: x for %d (%d) = %d, y = %d\n", i + 1, 
					pid[i], parentbuf.xvalue, parentbuf.yvalue);
			msgsnd(msqid, &parentbuf, sizeof(struct msqbuf_parent) - 
					sizeof(long), 0);
			n = msgrcv(msqid, &childbuf[i], 
						sizeof(struct msqbuf_child) - sizeof(long), 
						pid[i], IPC_NOWAIT);
			if(n != sizeof(struct msqbuf_child) - sizeof(long)) 
				continue;
			if(outofbounds(&childbuf[i])) {
				printf("Ball %d (%d) went out with %d changes\n", i + 1,
						childbuf[i].pid, childbuf[i].steps);
				if(kill(childbuf[i].pid, SIGTERM) == -1){
					perror("SIGTERM");
					exit(1);
				}			
				all--;
			}	
		}
		if(all == 0) break;
	}
	free(childbuf);
}

int main(int argc, char ** argv)
{
	if(argc < 2) {
		perror(USAGE);
		exit(0);
	}
	int count = atoi(argv[1]);
	int i;
		
	// memory allocation
	pid_t * pid;
	pid = (pid_t *)calloc(count, sizeof(pid_t));
	int msqid = getqueue();
	
	// start "balls"
	for(i = 0; i < count; i++) {
		pid[i] = fork();
		if(pid[i] == 0)	{
			fchild(i + 1, msqid);
			exit(0);
		}
	}
	fparent(msqid, pid, count);
	free(pid);
	msgctl(msqid, IPC_RMID, NULL);
	printf("All were done. Memory is free\n");
	return 0;
}
