#ifndef FUNCTION_H
#define FUNCTION_H
#include "functions.h"
#endif // FUNCTION_H

#ifndef AMESSAGE_H
#define AMESSAGE_H
#include "amessage.pb-c.h"
#endif // AMESSAGE_H

/* файл с обертками функций. В обертке проводится проверка на ошибки.
 * Имя обертки = имя функции с заглавной буквы, так, обертка для 
 * func(...) будет называться Func(...). */
 
struct sembuf lock_mem = { 0, -1, 0 };	//блокировка ресурса (нулевой, доступность для процессов -1, ждать)

struct sembuf ulock_mem = { 0, 1, 0 };	//освобождение ресурса (нулевой, увеличить доступность на 1, ждать)

void block(int semid)
{
	int block = semop(semid, &lock_mem, 1);
	if(block == -1){
		perror("lock");
		exit(1);
	}
}

void unblock(int semid)
{
	int unblock = semop(semid, &ulock_mem, 1);
	if(unblock == -1){
		perror("unlock");
		exit(1);
	}
}

void * threadFcn(void * Arg)
{
	struct thread_arg * arg;
	int semid, shmid, * shm_count;
	//arg = (struct thread_arg *) Arg;
	//semid = arg->semid;
	//*shm_count[0] = arg->shm_count;
	
	key_t key = ftok("/etc/sysctl.conf", 1);
	shmid = Shmget(key, 1, 0); 
	shm_count = (int *)shmat(shmid, NULL, 0);
	semid = semget(key, 1, 0);
	
	while(1) {
		arg = (struct thread_arg *) Arg;
		//pthread_mutex_lock(&mutex);
		block(semid);
		if ( shm_count[0] < 5 ) {
				//pthread_mutex_unlock(&mutex);
				unblock(semid);
				printf("brd: %d clients\n", shm_count[0]);
				/* serialize, pack data */
				AMessage msg = AMESSAGE__INIT;
				void *buf;
				unsigned len;
				pthread_mutex_lock(&mutex);
				msg.clients_count = shm_count[0];
				pthread_mutex_unlock(&mutex);
				msg.send_str = arg->sendString;
				//msg.listener = arg->listener;
				len = amessage__get_packed_size(&msg);
				buf = malloc(len);
				amessage__pack(&msg,buf);
				
				//buf = "random";
				//len = sizeof(buf);
				
				if (sendto(arg->sock, buf, 
						len, 0, (struct sockaddr *) 
						&(arg->broadcastAddr), 
						sizeof(arg->broadcastAddr)) != 
						len)
				 DieWithError("sendto() sent a different number of bytes than expected");

				printf("сервер ждет сообщений, клиенты уведомлены.\n", len);
				sleep(3);   /* Avoids flooding the network */
			}
		else { 
			//pthread_mutex_unlock(&mutex);
			unblock(semid);
			printf("сервер загружен, широковещалка off\n");
			sleep(10);
		}
	}
}

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

void Pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                          void *(*start_routine) (void *), void *arg)
{
	if (pthread_create(thread, attr, start_routine, arg) != 0)
            DieWithError("pthread_create() failed");
}

int Socket(int domain, int type, int proto)
{
	int sock;
	if ((sock = socket(domain, type, proto)) < 0)
        DieWithError("socket() failed");
    return sock;
}

int Setsockopt(int sockfd, int level, int optname,
                      const void *optval, socklen_t optlen)
{
	int sso;
    if ((sso = setsockopt(sockfd, level, optname, (void *) &optval, 
          sizeof(optlen))) < 0)
        DieWithError("setsockopt() failed");
    return sso;
}
int Bind(int socket, const struct sockaddr *address,
              socklen_t address_len)
{
	int bnd;
    if ((bnd = bind(socket, (struct sockaddr *) address, 
			(socklen_t)sizeof(*address))) < 0)
        DieWithError("bind() failed");
    return bnd;
}

int Listen(int socket, int backlog)
{
	int lstn;
    if ((lstn = listen(socket, backlog)) < 0)
        DieWithError("listen() failed");
    return lstn;
}

int Accept(int socket, struct sockaddr * address,
              socklen_t * address_len)
{
	int accpt;
	if ((accpt = accept(socket, (struct sockaddr *) address, 
							address_len)) < 0)
            DieWithError("accept() failed");
    return accpt;
}

ssize_t Recvfrom(int socket, void * buffer, size_t length,
              int flags, struct sockaddr * address,
              socklen_t * address_len)
{
	ssize_t rcvFrm;
	if((rcvFrm = recvfrom(socket, buffer, length, flags, address, 
							address_len)) < 0)
			DieWithError("recvfrom() failed");
	return rcvFrm;
}

void InterruptSignalHandler(int signalType)
{
	//shutdown(sig_thr_arg.sock, SHUT_RDWR);
	//shmdt(&shm_count);
	if (sig_thr_arg.sock) close(sig_thr_arg.sock);
	//if (sig_thr_arg.sock_cli) close(sig_thr_arg.sock_cli);
    printf("\nServer terminated by signal.\n");
    exit(1);
}

void * sigHandleFcn (void * arg)
{
	struct sigaction handler;    /* Signal handler specification structure */

    /* Set InterruptSignalHandler() as handler function */
    handler.sa_handler =  InterruptSignalHandler;
    /* Create mask that mask all signals */
    if (sigfillset(&handler.sa_mask) < 0) 
        DieWithError("sigfillset() failed");
    /* No flags */
    handler.sa_flags = 0;

    /* Set signal handling for interrupt signals */
    if (sigaction(SIGINT, &handler, 0) < 0)
        DieWithError("sigaction() failed");

    for(;;)
        pause();  /* suspend program until signal received */
}

int Connect(int sockfd, const struct sockaddr *addr,
                   socklen_t addrlen)
{
	int conn;
	if((conn = connect(sockfd, addr, addrlen)) < 0)
		DieWithError("connect() failed");
	return conn;
}

int Shmget(key_t key, size_t size, int shmflg)
{
	int sh;
	if( ( sh = shmget( key, size, shmflg ) ) < 0 )
		DieWithError("shmget() failed");
	return sh;
}

void * Shmat(int shmid, const void *shmaddr, int shmflg)
{
	void * sh = shmat(shmid, shmaddr, shmflg);
	if(sh == (int *)-1)
		DieWithError("Shmat() failed");
	return sh;
}





