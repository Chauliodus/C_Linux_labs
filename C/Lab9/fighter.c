/* fighter.c */

#ifndef RAWCONFIG_H
#include "rawconfig.h"

int main(int argc, char ** argv)
{
	if(argc < 2){
		perror("Too few arguments - fighter");
		exit(1);
	}
	int mynum = atoi(argv[1]);
	int shmid_ring, semid, shmid_schedule;
	getmemory(&shmid_ring, &semid, &shmid_schedule, 0);
	
	int pid_in_sch;
	int mynum_in_sch;
	int enemynum, enemypid;
	int damage, mod;
	
	srand(getpid());
	while(1)
	{
		
		while(1){
			pid_in_sch = ((schedule[LEFT_PID] == getpid()) || (schedule[RIGHT_PID] == getpid()));
			mynum_in_sch = ((schedule[LEFT] == mynum) || (schedule[RIGHT] == mynum));
			if((pid_in_sch && mynum_in_sch) || (schedule[GAME_OVER] == 1)){
				if (schedule[GAME_OVER] == 1) {
					shmdt(ring);
					shmdt(schedule);
					exit(0);
				} else break;
			}
		}
		
		if(schedule[LEFT] == mynum){
			enemynum = schedule[RIGHT];
			enemypid = schedule[RIGHT_PID];
			mod = -1;
		} 
		if(schedule[RIGHT] == mynum) {
			enemynum = schedule[LEFT];
			enemypid = schedule[LEFT_PID];
			mod = 1;
		}
				
		if(mod == -1) schedule[L_READY] = 1; 
		if(mod == 1) schedule[R_READY] = 1;
		printf("%d (%d) is ready to fight! Wait for my enemy %d (%d)\n", 
					getpid(), mynum, enemypid, enemynum);
		
		while(1){ // ждет соперника
			if(mod == -1)
				if(schedule[R_READY] == 1){
					printf("%d (%d) goes to the ring\n", getpid(), mynum);
					break;
				} 
			if(mod == 1)
				if(schedule[L_READY] == 1){
					printf("%d (%d) goes to the ring\n", getpid(), mynum);
					break;
				}
		}	
		
		while(1){
			block(semid);
			if(abs(*ring) >= TEAMSIZE) {
				unblock(semid);
				break;
			}
			damage = rand()%TEAMSIZE * mod;
			printf("(%d (%d)) damage: %d, ", mynum, getpid(), damage);
			*ring += damage;
			printf("result: %d\n", *ring);
			if(abs(*ring) >= TEAMSIZE) {
				unblock(semid);
				break;
			}
			unblock(semid);
			sleep(1);
		}
		if(mod == -1){
			schedule[L_FINISH] = 1;
			if(*ring > 0) {
				shmdt(ring);
				shmdt(schedule);
				exit(0);
			}
			schedule[L_WAITS] = 1;
		}
		if(mod == 1){
			schedule[R_FINISH] = 1;
			if(*ring < 0) {
				shmdt(ring);
				shmdt(schedule);
				exit(0);
			}
			schedule[R_WAITS] = 1;
		}

		sleep(1);
		
		while(1) 
			if(((schedule[R_WAITS] == -1) && (schedule[L_WAITS] == -1)) 
			|| (schedule[GAME_OVER] == 1)) {
				if(schedule[GAME_OVER] == 1){
					shmdt(ring);
					shmdt(schedule);
					exit(0);
				} else break;
			}
	}

	exit(0);
}
#endif
