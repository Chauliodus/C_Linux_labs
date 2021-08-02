#ifndef FUNCTION_H
#define FUNCTION_H
#include "functions.h"
#endif // FUNCTION_H

#define COUNT 20
#define USAGE "USAGE : ./course <COUNT>, COUNT = 5 clients by default\n"

int main(int argc, char ** argv)
{
	pid_t * pids;
	int i, count = COUNT;
	char arg[1];
	
	printf(USAGE);
	if( argc > 1 ) count = atoi(argv[1]);
	
	pids = calloc(count, sizeof(pid_t));
	for(i = 0; i < count; i++){
		pids[i] = fork();
		if (pids[i] == 0) {
			sprintf(arg, "%d", i + 2);
			execl("./course_cli_1", "course_cli_1", arg, NULL);
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
