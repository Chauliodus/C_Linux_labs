/* pooh.c */
/* Code for execl() in lab7. Describes behavior of Pooh. 
* written by Alex Shved for Eltex Programing School 6.11.19
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>

int main(int argc, char * argv[]) 
{
	if(argc < 3)
	{
		perror("Usage: ./pooh survivability hunger");
		exit(1);
	}
	const int survivability = atoi(argv[1]);
	const int hunger = atoi(argv[2]);
	
	int barrel = open("barrel", O_RDWR);
	if(barrel <= 0)
	{
		perror("pooh log");
		exit(1);
	}
	
	int log = open("LOG", O_WRONLY | O_APPEND);
	if(log <= 0)
	{
		perror("pooh log");
		exit(1);
	}
	
	char buff[80] = { 0 };
	char bufff[10] = { 0 };
	
	if(barrel <= 0)
	{
		perror("pooh cannot open barrel");
		exit(0);
	}
	
	int total = 0;
	int waiting_to_death = survivability;
	
	char honey[10] = { 0 };
	
	while(1)
	{
		usleep(waiting_to_death);
		
		// read "honey" from "barrel"
		lseek(barrel, 0, 0);
		lockf(barrel, F_LOCK, 10);
		read(barrel, honey, 10);
		memset(bufff, 0, 10);
		write(barrel, bufff, 10);
		if(atoi(honey) <= 0)
		{
			memset(buff, 0, 80);
			sprintf(buff, ":^3 bear see no honey in barrel.\n");
			printf("%s", buff);
			lseek(log, 0, 0);
			lockf(log, F_LOCK, 80);
			write(log, buff, sizeof(char) * strlen(buff));
			lseek(log, 0, 0);
			lockf(log, F_ULOCK, 80);
		}
		else
		{
			memset(buff, 0, 80);
			sprintf(buff, ":^3 bear see honey in barrel: %s\n", honey);
			printf("%s", buff);
			lseek(log, 0, 0);
			lockf(log, F_LOCK, 80);
			write(log, buff, sizeof(char) * strlen(buff));
			lseek(log, 0, 0);
			lockf(log, F_ULOCK, 80);
		}
	
		// assert honey amount is enough 
		int honey_int = atoi(honey);
	
		if(honey_int >= hunger)
		{
			// eat honey
			honey_int -= hunger;
			memset(buff, 0, 80);
			sprintf(buff, ":^D Bear ate %d honey. Now there are %d\n", 
															hunger,
															honey_int);
			printf("%s", buff);
			lseek(log, 0, 0);
			lockf(log, F_LOCK, 80);
			write(log, buff, sizeof(char) * strlen(buff));
			lseek(log, 0, 0);
			lockf(log, F_ULOCK, 80);
			total += hunger;
			
			// regenerate survivability
			if(waiting_to_death < survivability)
				waiting_to_death++;
				
			// put value back
			memset(honey, 0, 10);
			memset(bufff, 0, 10);
			sprintf(honey, "%d\n", honey_int);
			printf("%d", honey_int);
			lseek(barrel, 0, 0);
			write(barrel, bufff, sizeof(char) * strlen(bufff));
			lseek(barrel, 0, 0);
			write(barrel, honey, sizeof(char) * strlen(honey));
			lseek(barrel, 0, 0);
			lockf(barrel, F_ULOCK, 10);
		} 
		else 
		{
			lseek(barrel, 0, 0);
			lockf(barrel, F_ULOCK, 10);
			memset(buff, 0, 80);
			sprintf(buff, 
				"There is not enough honey (%d)! Wait %d seconds...\n", 
				honey_int, waiting_to_death);
			printf("%s", buff);
			lseek(log, 0, 0);
			lockf(log, F_LOCK, 80);
			write(log, buff, sizeof(char) * strlen(buff));
			lseek(log, 0, 0);
			lockf(log, F_ULOCK, 80);
			waiting_to_death--;
			
			if(waiting_to_death <= 0)
			{
				memset(buff, 0, 80);
				sprintf(buff, "Bear is dead\n");
				printf("%s", buff);
				lseek(log, 0, 0);
				lockf(log, F_LOCK, 80);
				write(log, buff, sizeof(char) * strlen(buff));
				lseek(log, 0, 0);
				lockf(log, F_ULOCK, 80);
				close(barrel);
				close(log);
				exit(total);
			}
		}
	}
}
