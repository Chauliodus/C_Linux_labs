/* lab9.c */
/* Alexey Shved
 * There are <count> (2 - 10) of teams, which fight to each other by
 * pairs.*/

#ifndef RAWCONFIG_H
#include "rawconfig.h"

int main(int argc, char ** argv)
{
	if(argc < 2 || (atoi(argv[1]) > 10)){
		perror(USAGE);
		exit(1);
	}
	mtrace();
	const int count = atoi(argv[1]);
	int shmid_ring, semid, shmid_schedule;
	getmemory(&shmid_ring, &semid, &shmid_schedule, 1);
	schedule[LEFT_PID] = -1;
	schedule[RIGHT_PID] = -1;
	schedule[LEFT] = -1;
	schedule[RIGHT] = -1;
	schedule[L_READY] = -1;
	schedule[R_READY] = -1;
	schedule[R_FINISH] = -1;
	schedule[L_FINISH] = -1;
	schedule[GAME_OVER] = -1;
	ring[0] = 0;
	pid_t * pid;
	pid = (pid_t *)calloc(count, sizeof(pid_t));
	createpids(pid, count);	
	int ** queue, maxround = getmaxround(count);
	queue = (int **)calloc(maxround + 1, sizeof(int *));
	int i, f = count;
	for(i = 0; i <= maxround; i++){
		queue[i] = (int *)calloc(f, sizeof(int));
		f = f / 2 + f % 2;
	}
	int * thisround;
	thisround = (int *)calloc(count, sizeof(int));
	for(i = 0; i < count; i++){
		queue[0][i] = pid[i];
		thisround[i] = i;
		schedule[i] = 0;
	}
	printqueue(queue, count);
	int j, round = 0, turn = 0, teamsalive = count;
	while(round < maxround){
		schedule[LEFT_PID] = queue[round][turn * 2];
		schedule[RIGHT_PID] = queue[round][turn * 2 + 1];
		schedule[LEFT] = thisround[turn * 2];
		schedule[RIGHT] = thisround[turn * 2 + 1];
		schedule[L_FINISH] = -1;
		schedule[R_FINISH] = -1;
		schedule[L_WAITS] = -1;
		schedule[R_WAITS] = -1;
		printf("ROUND %d, TURN %d: %d (%d) VS %d (%d)\n", round + 1, 
				turn + 1, schedule[LEFT_PID], schedule[LEFT], 
				schedule[RIGHT_PID], schedule[RIGHT]);
		while(1){
			if((schedule[R_FINISH] == 1) && (schedule[L_FINISH] == 1)){
				schedule[LEFT_PID] = -1;
				schedule[RIGHT_PID] = -1;
				schedule[LEFT] = -1;
				schedule[RIGHT] = -1;
				// winner goes to the next round
				if(*ring > 0) { 
					queue[round + 1][turn] = queue[round][turn * 2 + 1];
					thisround[turn] = thisround[turn * 2 + 1];
					printf("Turn done. Right team wins! %d (%d) goes %s", 
							queue[round][turn * 2 + 1], 
							thisround[turn * 2 + 1],
							"to the next round\n"); 
				}
				if(*ring < 0) {
					queue[round + 1][turn] = queue[round][turn * 2];
					thisround[turn] = thisround[turn * 2];
					printf("Turn done. Left team wins! %d (%d) goes %s", 
							queue[round][turn * 2], 
							thisround[turn * 2],
							"to the next round\n"); 
				}
				*ring = 0;
				turn++;
				teamsalive--;
				break;
			}
		}
		if(turn == (teamsalive - 1)) {
			queue[round + 1][turn] = queue[round][turn * 2];
			thisround[turn] = thisround[turn * 2];
			printf("%d (%d) as unpaired mooves to last place in the %s",
					queue[round][turn * 2], thisround[turn * 2],
					"next round\n");
		}
		if((turn == (teamsalive - 1)) || (turn == teamsalive)) {
			round++;
			turn = 0;
			/* into "thisround" goes numbers of alive teams according to
			 *  their PIDs */
			for(i = 0; i < count; i++) 
				for(j = 0; j < teamsalive; j++)
					if(queue[0][i] == queue[round][j])
						thisround[j] = i;
		}
		printqueue(queue, count);
	}
	schedule[GAME_OVER] = 1;
	printf("%d (%d) is winner!\n\n", queue[maxround][0], 
									 thisround[0]);
	for(i = 0; i < count; i++)
		wait(&pid[i]);
	freemem(pid, queue, count, thisround);
	exit(0);
}
#endif
