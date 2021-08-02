#include <stdio.h>
#include <stdlib.h> /* for exit() */
#include <string.h>  /* for strlen() */
#include <strings.h> /* for bzero() */
#include <netinet/in.h>
#include <sys/ipc.h> 
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <unistd.h>     /* for close() */
#include "amessage.pb-c.h"
#include <signal.h> /* for sigaction() */
#include <sys/un.h>


union semun{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
	struct seminfo *__buf;
} arg;



void * threadFcn(void * Arg);

pthread_mutex_t mutex;
                          
struct thread_arg {
	int 				sock;
	char *  			sendString;
	int 				sendStrLen;
	struct sockaddr_in 	broadcastAddr;
	//int 				clients_count;
	int					listener;
	int 				semid;
	int *				shm_count;
};

struct sigThreadArg {
	int sock;
	int sock_cli;
} sig_thr_arg;

void Pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                          void *(*start_routine) (void *), void *arg);
void DieWithError(char *errorMessage);
int Socket(int, int, int);
int Setsockopt(int sockfd, int level, int optname,
                      const void *optval, socklen_t optlen);
int Bind(int socket, const struct sockaddr *address,
              socklen_t address_len);
int Listen(int socket, int backlog);
int Accept(int socket, struct sockaddr *address, 
				socklen_t * address_len);
size_t read_buffer (unsigned max_length, uint8_t *out);
void InterruptSignalHandler(int signalType);
void * sigHandleFcn (void * arg);
