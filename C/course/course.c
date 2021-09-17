#ifndef FUNCTION_H
#define FUNCTION_H
#include "functions.h"
#endif // FUNCTION_H

#define USAGE "USAGE : ./course <COUNT_1> <COUNT_2>\n"

int main(int argc, char ** argv)
{
	pid_t * pids;
	int i, count_1 = COUNT_1, count_2 = COUNT_2;
	int count = count_1 + count_2;
	char arg[1];
	
	printf(USAGE);
	if( argc == 3 ) { 
		count_1 = atoi(argv[1]);
		count_2 = atoi(argv[2]);
	}
	
	pids = calloc(count, sizeof(pid_t));
	for(i = 0; i < count; i++){
		pids[i] = fork();
		if (pids[i] == 0) {
			sprintf(arg, "%d", i + 2);
			if ( i < count_1 ) execl("./course_cli_1", "course_cli_1", arg, NULL);
			if ( i >= count_1 ) execl("./course_cli_2", "course_cli_2", arg, NULL);
		}
	}
	int result, stat;
	for(i = 0; i < count; i++)
	{
		waitpid(pids[i], &stat, 0);
		result = WEXITSTATUS(stat);
		printf("%d did it; returned %d\n", pids[i], result);
	}
	free(pids);
	exit(0);
}
