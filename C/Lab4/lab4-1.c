/* lab4-1.c */
/* Written by Alex Shved for Eltex Programing School, 3 oct 2019.
 * A program making some changes in input file: for <count of changes>
 * times it adds a dot after a space. The result is written into the 
 * file with a type "out".
 * */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define USAGE "./lab4 <input file name> <count of changes>\n"

// find space - add dot - increase count of changes - check it is less
// than a limit.
size_t makeChange(uint8_t * buffer, size_t * count, size_t maxCount)
{
	uint8_t newBuf[strlen((char *)buffer) * 2]; // should I use memmove?
	memset(newBuf, '\0', strlen((char *)buffer) * 2);
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

// function parsing a file name, changes a type from given to "out"
void newName(const uint8_t * oldName, uint8_t * newName)
{
	uint8_t * buf;
	strcpy((char *)newName, (char *)oldName);
	buf = (uint8_t *)strstr((char *)oldName, ".");
	size_t nameLength = buf - oldName + 1;
	newName[nameLength] = (uint8_t)'o';
	newName[nameLength + 1] = (uint8_t)'u';
	newName[nameLength + 2] = (uint8_t)'t';
	newName[nameLength + 3] = (uint8_t)'\n';
	size_t i;
	for(i = nameLength + 3; i < strlen((char *)newName); i++)
		newName[i] = 0;
}

int main(int argc, char * argv[])
{
	if(argc < 3)
	{
		perror(USAGE);
		exit(1);
	}
	
	FILE * inputFile, * outputFile;
	if((inputFile = fopen(argv[1], "r")) == NULL)
	{
		perror("Cannot open input file");
		exit(1);
	}
	
	size_t nNameLength = 20;
	uint8_t nName[nNameLength];
		
	newName((uint8_t *)argv[1], nName);
	if((outputFile = fopen((char *)nName, "w+")) == NULL)
	{
		perror("Cannot open output file");
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
		fputs((char *)buf, outputFile);
	}
	
	if(fclose(inputFile))
	{
		perror("Cannot close input file");
		exit(1);
	}
	
	if(fclose(outputFile))
	{
		perror("Cannot close output file");
		exit(1);
	}
	
	return 0;
}
