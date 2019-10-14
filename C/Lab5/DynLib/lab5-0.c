/* lab5-0.c */
/* Written by Alex Shved for Eltex Programing School, 10 oct 2019.
 * Example of using libraries.
 * */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <inttypes.h>
#include <dlfcn.h>

int main(int argc, char ** argv)
{
	void * dl_handler;
	dl_handler = dlopen("liblab5.so", RTLD_LAZY);
	if(!dl_handler) 
	{ 
		fprintf(stderr, "lib failed: %s\n", dlerror()); 
		exit(1); 
	}
	
	//получениe адреса требуемой функции из библиотеки:
	uint8_t (*f1)(const uint8_t, const uint8_t);
	*(void **)(&f1) = dlsym(dl_handler, "adding_uint8_t");
	double_t (*f2)(const double_t, const double_t);
	*(void **)(&f2) = dlsym(dl_handler, "adding");
	
	uint8_t sum1;
	sum1 = f1(5, 7);
	printf("%d\n", (int)sum1);
	
	double_t sum2;
	sum2 = f2(15, 14);
	printf("%Lg\n", sum2);
	
	dlclose(dl_handler);
	
	return 0;
}
