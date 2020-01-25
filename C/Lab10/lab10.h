/* lab10.h */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <time.h>
#include <mcheck.h>

#define USAGE "<count> <minsize> <maxsize> <minvalue> <maxvalue>"

void * calculate(void * arg);
double shared;
pthread_mutex_t mutex;
