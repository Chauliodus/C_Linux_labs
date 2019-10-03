#include <stdio.h>
#include <unistd.h>
int main(int argc, char** argv)
{
int i;
for(i = 0; i < 5; i++){	
printf("Hello, world-%d!\n", i);
	sleep(1);}
	return 0;
} 
