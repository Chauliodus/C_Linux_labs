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
 
void * threadFcn(void * Arg)
{
	struct thread_arg * arg;
	while(1) {
		pthread_mutex_lock(&mutex);
		arg = (struct thread_arg *) Arg;
		if ( arg->clients_count < 5 ) {
				pthread_mutex_unlock(&mutex);
				
				/* serialize, pack data */
				AMessage msg = AMESSAGE__INIT;
				void *buf;
				unsigned len;
				msg.clients_count = arg->clients_count;
				msg.send_str = arg->sendString;
				msg.listener = arg->listener;
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
			pthread_mutex_unlock(&mutex);
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
	if (sig_thr_arg.sock) close(sig_thr_arg.sock);
	if (sig_thr_arg.sock_cli) close(sig_thr_arg.sock_cli);
    printf("close %d.\n", sig_thr_arg.sock);
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
