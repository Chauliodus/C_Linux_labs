#ifndef FUNCTION_H
#define FUNCTION_H
#include "functions.h"
#endif // FUNCTION_H

#define MAX_MSG_SIZE 	1024  /* Longest msg to receive */

int main(int argc, char *argv[])
{
    int sock, connSock, i;                         /* Socket */
    struct sockaddr_in broadcastAddr; /* Broadcast Address */
    struct sockaddr_in connectServAddr; /* server address */
    unsigned short broadcastPort = PORT;     /* Port */
    unsigned short connectPort = PORT;     /* Port */
    pthread_t 				sig_handle_thread;
    
    char addr[10];
    sprintf(addr, "127.0.0.%s", argv[1]);
    printf("cli 1 %s\n", addr);
    
    srand(time(NULL) - getpid());
    AMessage *msg;
    
    uint8_t buf[MAX_MSG_SIZE];
	size_t msg_len;

    /* Create a best-effort datagram socket using UDP */
    sock = Socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    /* Construct bind structure */
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));   /* Zero out structure */
    broadcastAddr.sin_family = AF_INET;                 /* Internet address family */
    broadcastAddr.sin_addr.s_addr = htonl(INADDR_ANY); //inet_addr(addr);//inet_addr("127.0.0.2"); //  /* Any incoming interface */
    broadcastAddr.sin_port = htons(broadcastPort);      /* Broadcast port */

	int reUseAddrLstr = 1;
	Setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &reUseAddrLstr, 
					sizeof(reUseAddrLstr));    /* permit reuse addr */
					
	memset(&connectServAddr, 0, sizeof(connectServAddr));   /* Zero out structure */
    connectServAddr.sin_family = AF_UNIX;                 /* Internet address family */
    connectServAddr.sin_addr.s_addr = inet_addr(addr);//inet_addr("127.0.0.2"); //  /* Any incoming interface */
    connectServAddr.sin_port = htons(connectPort);

    //sprintf(connectServAddr.sun_path, "/tmp/9Lq7BNBnBycd6nxy1.socket");



	
	//int reuse = 1;
	//Setsockopt(connSock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &reuse, 					sizeof(reuse));
	//Bind(connSock, (const struct sockaddr *) &connectServAddr,               sizeof(connectServAddr));
					
    /* Bind to the broadcast port */
    Bind(sock, (struct sockaddr *) &broadcastAddr, sizeof(broadcastAddr));
    
    sig_thr_arg.sock_cli = connSock; // Чтоб сокет закрывался при Ctrl+C
	Pthread_create(&sig_handle_thread, NULL, sigHandleFcn, NULL);
    
    while(1) {
		msg_len = Recvfrom(sock, buf, MAX_MSG_SIZE, 0, NULL, 0);
		//printf(buf);

		msg = amessage__unpack(NULL, msg_len, buf);
		
		if (msg == NULL){
			printf("error unpacking incoming message\n");
			exit(1);
		}
		
		printf("получено %s\n", msg->send_str);
		
		if( strcmp( msg->send_str, "Жду сообщений" ) == 0 ) {
			connSock = Socket(AF_UNIX, SOCK_STREAM, 0);
			Connect(connSock, (struct sockaddr *) &connectServAddr,
								sizeof(connectServAddr));
			char bla[25];
			for (i = 0; i < 18; i++)
				bla[i] = 'a' + rand()%26;
			bla[i] = '\0';
			//printf(bla, "%s - from %s", bla, argv[1]);
			if(send(connSock, bla, sizeof(bla), 0) < sizeof(bla)) DieWithError("cli_1: send() failed");
			sleep(6);
			close(connSock);
		}
	}
    
    amessage__free_unpacked(msg, NULL);
	close(connSock);
	close(sock);

    exit(0);
}
