/* Запускает указанное число клиентов, либо 5 по умолчанию.

"Шарики"

Каждый шарик - клиент. У шарика есть координаты x и y, которые
должны быть в пределах 800*600. Изначально - по середине.

Клиент подключается к серверу, сообщает свой пид и получает 
случайное число от -100 до 100. Число прибавляется к x и y,
процесс повторяется, пока шарик в пределах границ. */

#include <unistd.h>
#include <sys/wait.h>

#define COUNT 5

int main(int argc, char ** argv)
{
	int 		count = COUNT, result, stat, i;
	pid_t * 	pids;
	
	printf("USAGE : ./cli_few <COUNT>, COUNT = 5 clients by default\n");
	if( argc > 1 ) count = atoi(argv[1]);
	
	pids = calloc(count, sizeof(pid_t));

	for(i = 0; i < count; i++){
		pids[i] = fork();
		if (pids[i] == 0) execl("./lab11_client", "lab11_client", NULL);
	}
	
	for(i = 0; i < count; i++)
	{
		waitpid(pids[i], &stat, 0);
		result = WEXITSTATUS(stat);
		printf("\x1b[0m%d did it; returned %d\n", pids[i], result);
	}
	
	free(pids);
	
	exit(0);
}
 
