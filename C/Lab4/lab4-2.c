/* lab4-2.c */
/* Written by Alex Shved for Eltex Programing School, 3 oct 2019.
 * The program removes non-latin symbols from input file. The result 
 * is written into the file with a type "out".
 * */
 
 // ЭТОЙ СТРОЧКИ БЫТЬ НЕ ДОЛЖНО!

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

// и этой тоже 567920"№;%:?*(
#define USAGE "./lab4 <input file name>\n"

// if symbol in needed interval, the function puts it into output file
size_t makeChange(uint8_t buf, FILE * outp)
{
	if(((char)buf >= 'a' && (char)buf <= 'z') || ((char)buf >= 'A' && (char)buf <= 'Z'))
		fputc(buf, outp);
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
	if(argc < 2)
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
	
	uint8_t buf;
	while(!feof(inputFile))
	{
		buf = fgetc(inputFile);
		if(makeChange(buf, outputFile) != 0)
		{
			perror("makeChange failed");
			exit(1);
		}
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
