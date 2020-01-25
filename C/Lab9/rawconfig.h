/* rawconfig.h */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <math.h>
#include <mcheck.h>
#define COUNT 	 		5 // число команд
#define MAX_COUNT		100
#define TEAMSIZE		6 // размер каждой команды
#define LEFT			200  // schedule[LEFT] - н.п.п. "левой" (с отрицательным модификатором) команды
#define RIGHT			201  // номер "правой" (с положительным модификатором) команды
#define LEFT_PID		202  // PID "левой" команды
#define RIGHT_PID		203  // PID "правой" команды
#define GAME_OVER		204  // индекс флага "турнир окончен"
#define L_READY			205  // индекс элемента с отметкой, что "левая" команда готова зайти на ринг
#define R_READY			206  // индекс элемента с отметкой, что "правая" команда готова к бою
#define L_FINISH		207  // --/-- "левая" команда освободила ринг
#define R_FINISH		208  // --/-- "правая" команда освободила ринг
#define R_WAITS			209  // --/-- "правая" команда ждет
#define L_WAITS			210  // --/-- "левая" команда ждет
#define USAGE 			"./lab9 <count (no more than 10)>"
struct sembuf lock_mem = { 0, -1, 0 };	//блокировка ресурса (нулевой, доступность для процессов -1, ждать)
struct sembuf ulock_mem = { 0, 1, 0 };	//освобождение ресурса (нулевой, увеличить доступность на 1, ждать)
union semun{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
	struct seminfo *__buf;
} arg;
void block(int semid)
{
	int block = semop(semid, &lock_mem, 1);
	if(block == -1){
		perror("lock");
		exit(1);
	}
}
void unblock(int semid)
{
	int unblock = semop(semid, &ulock_mem, 1);
	if(unblock == -1){
		perror("unlock");
		exit(1);
	}
}
int *schedule, *ring;
void getmemory(int *shmid_ring, int *semid, int *shmid_schedule, int parent)
{
	key_t key_ring = ftok("/etc/sysctl.conf", 0);
	key_t key_sch = ftok("/etc/sysctl.conf", 1);
	// create ring
	if(parent){
		*shmid_ring = shmget(key_ring, 1, IPC_CREAT | 0660);
		if(*shmid_ring < 0){
			perror("create shmid_ring");
			exit(1);
		}
	} else {
		*shmid_ring = shmget(key_ring, 1, 0);
		if(*shmid_ring < 0){
			perror("create shmid_ring");
			exit(1);
		}
	}
	ring = shmat(*shmid_ring, NULL, 0);
	if(ring == (int *)-1){
		perror("shmat ring");
		exit(1);
	}
	// create schedule
	if(parent){
		*shmid_schedule = shmget(key_sch, 1, IPC_CREAT | 0660);
		if(*shmid_schedule < 0){
			perror("create shmid_schedule");
			exit(1);
		}
	} else {
		*shmid_schedule = shmget(key_sch, 1, 0);
		if(*shmid_schedule < 0){
			perror("create shmid_schedule");
			exit(1);
		}
	}
	schedule = shmat(*shmid_schedule, NULL, 0);
	if(schedule == (int *)-1){
		perror("shmat schedule p");
		exit(1);
	}	
	// create semaphore
	if(parent){
		*semid = semget(key_ring, 1, IPC_CREAT | 0660);
		if(*semid < 0){
			perror("create semid");
			exit(1);
		}
	} else {
		*semid = semget(key_ring, 1, 0);
		if(*semid < 0){
			perror("create semid");
			exit(1);
		}
	}
	arg.val = 1;
	semctl(*semid, 0, SETVAL, arg);
	return;
}
int getmaxround(int count)
{
	int maxround = 0;
	while(count > 1){
		count = count / 2 + count % 2;
		maxround++;
	}
	return maxround;
}

// parent

void createpids(pid_t * pid, int count)
{
	int i;
	char number[2];
	for(i = 0; i < count; i++){
		sprintf(number, "%d", i);
		pid[i] = fork();
		if(pid[i] == 0) execl("./fighter", "fighter", number, NULL);
	}	
}
void printqueue(int ** queue, int count)
{
	int maxround = getmaxround(count);
	int i, j, f = count;
	printf("\n     OUR FIGHTERS\n\n");
	for(i = 0; i <= maxround; i++){
		if (queue[i][0] != 0)
			printf("%d round\n\n", i + 1);
		for(j = 0; j < f; j++)
			if (queue[i][j] != 0)
				printf("Queue[%d][%d] = %d\n", i, j, queue[i][j]);
		f = f / 2 + f % 2;
		printf("\n");
	}
}
void freemem(pid_t * pid, int ** queue, int count, int * thisround)
{
	int maxround = getmaxround(count);
	int i;
	shmdt(ring);
	shmdt(schedule);
	free(pid);
	for(i = 0; i <= maxround; i++)
		free(queue[i]);
	free(queue);
	free(thisround);
}
