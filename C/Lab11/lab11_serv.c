#include <stdio.h>
#include <netinet/in.h> 		// для пространства адресов интернета
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>

#define PORT 			9999
#define MAXLINE 		80
#define LISTENQ 		5 		// max clients per time

int
main(int argc, char **argv)
{
	int					listenfd, connfd, int_pid, delta;
	pid_t				childpid;
	socklen_t			clilen;
	ssize_t				n;
	char				buf[MAXLINE], pid[5]; // for send and recieve

	struct sockaddr_in	cliaddr, servaddr;
	
	// escape-последовательности для красивого вывода
	char* type = "\x1b[0;3", * stop = "\x1b[0m"; 

	listenfd = socket(AF_UNIX, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_UNIX;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(PORT);

	bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	listen(listenfd, LISTENQ);
		
	for ( ; ; ) {
		clilen = sizeof(cliaddr);
		connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);

		if ( (childpid = fork()) == 0) {	/* child process */
			close(listenfd);				/* child closes listening socket */
			srand(time(NULL));
			
			/* process the request */
			// read() blocks process while it waits data:
			while ( (n = read(connfd, pid, MAXLINE)) > 0){              
				delta = rand()%100 - rand()%100;
				// write random number into buf for sending to client:
				sprintf(buf, "%d", delta); 			
				// send the number to client:
				write(connfd, buf, n);
				// color of string depends on client's pid:  
				int_pid = atoi(pid)%6 + 1;
				printf("%s%dm%s asked for %d%s\n", type, int_pid, pid, 
						delta, stop);
				if (n < 0 && errno == EINTR)
					continue;
				else if (n < 0)
					perror("read error");
			}
			exit(0);
		}
		close(connfd);			/* parent closes connected socket */
	}
} 
