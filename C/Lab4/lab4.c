/* lab4.c */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define USAGE "./lab4 <input file name> <count of changes>\n"

size_t makeChange(uint8_t * buffer, size_t * count, size_t maxCount)
{
	uint8_t newBuf[strlen((char *)buffer) * 2]; // should I use memmove?
	uint8_t sign = buffer[0];
	size_t i = 0, j = 0;
	bool write = true;
	while(sign != '\n')
	{
		sign = buffer[j];
		newBuf[i] = buffer[j];
		if((sign == ' ') && (write))
		{
			newBuf[i + 1] = '.';
			(*count)++;
			i++;
		}
		i++;
		j++;
		if (*count >= maxCount) write = false;
	}
	strcpy((char *)buffer, (char *)newBuf);
	return 0;
}

int main(int argc, char * argv[])
{
	if(argc < 3)
	{
		perror(USAGE);
		exit(1);
	}
	
	FILE * inputFile;
	if((inputFile = fopen(argv[1], "r+")) == NULL)
	{
		perror("Cannot open file");
		exit(1);
	}
	
	size_t changesCount = 0;
	
	uint8_t buf[100];
	while(!feof(inputFile))
	{
		fgets((char *)buf, 80, inputFile);
		if(makeChange(buf, &changesCount, (size_t)atoi(argv[2])) != 0)
		{
			perror("makeChange failed");
			exit(1);
		}
		printf("%s\n", (char *)buf);
	}
	
	if(fclose(inputFile))
	{
		perror("Cannot close file");
		exit(1);
	}
	
	
	return 0;
}
