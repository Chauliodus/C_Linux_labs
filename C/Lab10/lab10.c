/* lab10.c */
/* Alexey Shved 10 01 2020 */
/* Статистический анализ. Имеется несколько массивов данных 
 * (разного размера). Требуется определить математическое ожидание 
 * в каждом массиве. Обработка каждого массива выполняется в отдельном 
 * процессе. */
 
 /* В переменную shared пишется среднее мат.ожидание по всем массивам.
  * Это бесполезно и прикручено только ради небольшой практики с 
  * мьютексами. */

#ifndef LAB10_H
#include "lab10.h"

void * calculate(void * arg)
{
	size_t * array = (size_t *)arg;
	size_t size = 0, i = 0;
	while(1){
		if(array[i] == -1) break;
		size++;
		i++;
	}
	double * mathw = (double *)malloc(sizeof(double));
	for(i = 0; i < size; i++)
		*mathw += (double)array[i] * (1.0 / (double)size);
	pthread_mutex_lock(&mutex);
	shared += *mathw;
	pthread_mutex_unlock(&mutex);
	pthread_exit((void *)mathw);
}
int main(int argc, char ** argv)
{
	if(argc < 6){
		perror(USAGE);
		exit(1);
	}
	size_t count = (size_t)atoi(argv[1]);
	size_t minsize = (size_t)atoi(argv[2]);
	size_t maxsize = (size_t)atoi(argv[3]) + 1;
	size_t minvalue = (size_t)atoi(argv[4]);
	size_t maxvalue = (size_t)atoi(argv[5]) + 1;
	char str1[50], str2[50], str3[50];
	sprintf(str1, "Count of arrays: %d.", count);
	sprintf(str2, "Size of array: %d-%d.", minsize, maxsize - 1);
	sprintf(str3, "Values in the arrays: %d-%d.", minvalue, 
			maxvalue - 1);
	printf("%s %s %s\n", str1, str2, str3);
	mtrace();
	srand(time(NULL));
	if(pthread_mutex_init(&mutex, NULL) != 0){
		perror("mutex init");
		exit(1);
	}
	pthread_t * thread;
	size_t ** array; // each array has size, written in size_t * size
	size_t * size;
	size_t i, j;
	array = (size_t **)malloc(count * sizeof(size_t *));
	size = (size_t *)malloc(count * sizeof(size_t));
	for(i = 0; i < count; i++){
		size[i] = rand()%(maxsize - minsize) + minsize;
		array[i] = (size_t *)malloc((size[i] + 1) * sizeof(size_t));
		for(j = 0; j < size[i]; j++)
			array[i][j] = rand()%(maxvalue - minvalue) + minvalue;
		array[i][size[i]] = -1;
	}
	thread = (pthread_t *)malloc(count * sizeof(pthread_t));
	for(i = 0; i < count; i++)
		if(pthread_create(&thread[i], NULL, calculate, array[i]) != 0){
			perror("thread1 create");
			exit(1);
		}
	void * res;
	for(i = 0; i < count; i++){
		if(pthread_join(thread[i], &res) != 0){
			perror("join thread");
			exit(1);
		}
		printf("array[%d] size: %d\n", i, size[i]);
		for(j = 0; j < size[i]; j++)
			printf("%d ", array[i][j]);
		printf("\n");
		printf("Thread %d returned mathwait: %f\n", i, 
				*((double *)res));
				free(res);
		printf("\n");
	}
	printf("Average mathwait is %f\n", shared / (double)count);
	free(size);
	free(thread);
	for(i = 0; i < count; i++)
		free(array[i]);
	free(array);
	return 0;
}
#endif
