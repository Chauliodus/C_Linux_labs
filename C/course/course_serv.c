#ifndef FUNCTION_H
#define FUNCTION_H
#include "functions.h"
#endif // FUNCTION_H

#define MAXLINE 		80

int main(int argc, char **argv)
{
	int						br_sender, listenfd, connfd, shmid, semid, * shm_count, tmp, str_i = 0, list_is_full = 0, x = 0;
	pid_t					childpid;
	char					buf[MAXLINE];
	char **					strings;
	struct sockaddr_in		broadcastAddr, servaddr;	
	struct sockaddr_un		cliaddr;
	pthread_t 				broadcastThread, sig_handle_thread;
	char *					broadcastIP = "127.255.255.255"; // 192.168.43.255 /* INADDR_BROADCAST ? */ /* ifconfig lo broadcast 127.255.255.255 --- note: brd + loopback = сомнительный результат (private addresses space? listen?.. read answers https://serverfault.com/questions/421389/how-to-add-a-broadcast-address-to-loopback-with-ifconfig-on-a-os-x) */
	int 					broadcastPermission = 1, reUseAddrBr = 1,
								reUseAddrLstr;
	struct thread_arg * 	brdThreadArg;
	key_t 					key = ftok("/etc/sysctl.conf", 1);
	

	shmid = Shmget(key, 1, IPC_CREAT | 0660); 
	shm_count = (int *)shmat(shmid, NULL, 0);
	semid = semget(key, 1, IPC_CREAT | 0660);
	if(semid < 0)
		DieWithError("Semget() failed");
	arg.val = 1;
	semctl(semid, 0, SETVAL, arg);
	
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
	//brdThreadArg->sendString = "Жду сообщений";
	//brdThreadArg->sendStrLen = strlen(brdThreadArg->sendString);
	brdThreadArg->broadcastAddr = broadcastAddr;
	brdThreadArg->semid = semid;
	shm_count[0] = 0;
	brdThreadArg->listener = listenfd;
	brdThreadArg->shm_count = shm_count;
	Pthread_create(&broadcastThread, NULL, threadFcn, brdThreadArg);
		
	/* listener */
	
	listenfd = Socket(AF_UNIX, SOCK_STREAM, 0);
	Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &reUseAddrLstr, 
					sizeof(reUseAddrLstr));    /* permit reuse addr */
	sig_thr_arg.sock = listenfd; // Чтоб сокет закрывался при Ctrl+C
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
	strings = calloc(20, 100);
	
	for ( ; ; ) {
		block(semid);
		if(shm_count[0] < LISTENQ){
			unblock(semid);
			connfd = Accept(listenfd, NULL, NULL);//(struct sockaddr *) &cliaddr, //
								//(socklen_t *)sizeof(cliaddr));
			
			//pthread_mutex_lock(&mutex);
			
			block(semid);
			shm_count[0]++;
			printf("queue = %d\n", shm_count[0]);
			//pthread_mutex_unlock(&mutex);
			unblock(semid);
			
			if ( (childpid = fork()) == 0) {	/* child process */
							
				close(listenfd);
				srand(time(NULL) - getpid());		
				shmid = Shmget(key, 1, 0);
				shm_count = (int *)shmat(shmid, NULL, 0);
				semid = semget(key, 1, 0);
				if(semid < 0)
					DieWithError("Semget() failed");
				
				char buff[25];
				bzero(buff, 25);
				//Recvfrom(connfd, buff, 18, NULL, (struct sockaddr *) &cliaddr, (socklen_t *)sizeof(cliaddr));
				recv(connfd, buff, 25, 0);
				
				printf("buff == %s\n", buff);
				if(strcmp(buff, "str") != 0) {
					strings[str_i] = buff;
					printf("От клиента получена строка: %s\n", strings[str_i]);
					if(str_i < MAXSTRINGS) str_i++; else { str_i = 0; list_is_full = 1; }
				} 
				
				if(strcmp(buff, "str") == 0) {
					if(list_is_full < 1) {
						printf("-------------\n");
						x = rand()%str_i; 
						printf("%d\n", x);
					} else { 
						x = rand()%MAXSTRINGS; 
						printf("%d\n", x);
					}
					printf("%d\n", x);
					printf("Клиенту отправлено: %s\n", strings[x]);
					if( send( connfd, strings[x], sizeof(strings[x]), 0 ) < sizeof(strings[x]) ) 
						DieWithError("serv: send() failed");
				}

				block(semid);
				shm_count[0]--;
				unblock(semid);
				
				exit(0);
			}
			close(connfd);			/* parent closes connected socket */
		} else unblock(semid);
	}
}
