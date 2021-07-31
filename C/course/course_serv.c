#ifndef FUNCTION_H
#define FUNCTION_H
#include "functions.h"
#endif // FUNCTION_H

#define PORT 			9998
#define MAXLINE 		80
#define LISTENQ 		5

int main(int argc, char **argv)
{
	int						br_sender, listenfd, connfd;
	pid_t					childpid;
	char					buf[MAXLINE];
	struct sockaddr_in		broadcastAddr, cliaddr, servaddr;	
	pthread_t 				broadcastThread, sig_handle_thread;
	char *					broadcastIP = "127.255.255.255"; // 192.168.43.255 /* INADDR_BROADCAST ? */ /* ifconfig lo broadcast 127.255.255.255 --- note: brd + loopback = сомнительный результат (private addresses space? listen?.. read answers https://serverfault.com/questions/421389/how-to-add-a-broadcast-address-to-loopback-with-ifconfig-on-a-os-x) */
	int 					broadcastPermission = 1, reUseAddrBr = 1,
								reUseAddrLstr;
	struct thread_arg * 	brdThreadArg;
	
	/* broadcast */
		
	br_sender = Socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	Setsockopt(br_sender, SOL_SOCKET, SO_BROADCAST, /* permit brdcst */
					(void *) &broadcastPermission, 
					sizeof(broadcastPermission));
	bzero(&broadcastAddr, sizeof(broadcastAddr));
	broadcastAddr.sin_family = AF_INET;
	broadcastAddr.sin_addr.s_addr = inet_addr(broadcastIP);
	broadcastAddr.sin_port = htons(PORT);
	brdThreadArg = (struct thread_arg *)malloc(sizeof(struct thread_arg));
	bzero(brdThreadArg, sizeof(struct thread_arg));
	brdThreadArg->sock = br_sender;
	brdThreadArg->sendString = "Жду сообщений";
	brdThreadArg->sendStrLen = strlen(brdThreadArg->sendString);
	brdThreadArg->broadcastAddr = broadcastAddr;
	brdThreadArg->clients_count = 0;
	brdThreadArg->listener = listenfd;
	Pthread_create(&broadcastThread, NULL, threadFcn, brdThreadArg);
		
	/* listener */
	
	listenfd = Socket(AF_UNIX, SOCK_STREAM, 0);
	Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &reUseAddrLstr, 
					sizeof(reUseAddrLstr));    /* permit reuse addr */
	sig_thr_arg.sock = listenfd; // Чтоб сокет закрывался при Ctrl+C
	printf("Listener: %d\n", sig_thr_arg.sock);
	Pthread_create(&sig_handle_thread, NULL, sigHandleFcn, NULL);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_UNIX;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");// htonl(INADDR_ANY); // 
	servaddr.sin_port        = htons(PORT);
	Bind(listenfd, (struct sockaddr *) &servaddr, 
						(socklen_t)sizeof(servaddr));
	Listen(listenfd, LISTENQ);
	
	/* main loop */
	
	bzero(&cliaddr, sizeof(cliaddr));
	
	for ( ; ; ) {
		connfd = Accept(listenfd, NULL, NULL);//(struct sockaddr *) &cliaddr, //
							//(socklen_t *)sizeof(cliaddr));
							
		pthread_mutex_lock(&mutex);
		brdThreadArg->clients_count++;
		pthread_mutex_unlock(&mutex);
		
		if ( (childpid = fork()) == 0) {	/* child process */
			close(listenfd);
			char buff[18];
			bzero(buff, 18);
			//Recvfrom(connfd, buff, 18, NULL, (struct sockaddr *) &cliaddr, (socklen_t *)sizeof(cliaddr));
			recv(connfd, buff, 18, NULL);
			printf("%s\n", buff);
			exit(0);
		}
		pthread_mutex_lock(&mutex);
		brdThreadArg->clients_count--;
		pthread_mutex_unlock(&mutex);
		close(connfd);			/* parent closes connected socket */
	}
}


