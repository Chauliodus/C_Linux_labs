#ifndef FUNCTION_H
#define FUNCTION_H
#include "functions.h"
#endif // FUNCTION_H

#define MAX_MSG_SIZE 	1024  /* Longest msg to receive */
#define PORT 			9998

int main(int argc, char *argv[])
{
    int sock, connSock;                         /* Socket */
    struct sockaddr_in broadcastAddr; /* Broadcast Address */
    struct sockaddr_in connectServAddr; /* server address */
    unsigned short broadcastPort = PORT;     /* Port */
    unsigned short connectPort = PORT;     /* Port */
    
    char addr[10];
    sprintf(addr, "127.0.0.%s", argv[1]);
    //printf(addr);
    
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



	connSock = Socket(AF_UNIX, SOCK_STREAM, 0);
	//int reuse = 1;
	//Setsockopt(connSock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &reuse, 					sizeof(reuse));
	//Bind(connSock, (const struct sockaddr *) &connectServAddr,               sizeof(connectServAddr));
					
    /* Bind to the broadcast port */
    Bind(sock, (struct sockaddr *) &broadcastAddr, sizeof(broadcastAddr));
    
    msg_len = Recvfrom(sock, buf, MAX_MSG_SIZE, 0, NULL, 0);
    //printf(buf);

    msg = amessage__unpack(NULL, msg_len, buf);
	
	if (msg == NULL){
		printf("error unpacking incoming message\n");
		exit(1);
	}
	
	close(sock);
	
	printf("%s\n", msg->send_str);
	
	if( strcmp( msg->send_str, "Жду сообщений" ) == 0 ) {
		Connect(connSock, (struct sockaddr *) &connectServAddr,
							sizeof(connectServAddr));
		char bla[18];
		sprintf(bla, "blabla%s", argv[1]);
		if(send(connSock, bla, sizeof(bla), 0) < sizeof(bla)) DieWithError("send() failed");
		sleep(6);
	}
    
    amessage__free_unpacked(msg, NULL);
	close(connSock);

    exit(0);
}
