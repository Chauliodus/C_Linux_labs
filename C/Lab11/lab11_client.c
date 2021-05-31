#include <stdio.h>
#include <netinet/in.h> // для пространства адресов интернета
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 			9999
#define MAXLINE 		80

#define BORDER_HORIZ 	800
#define BORDER_VERT 	600

int
main(int argc, char **argv)
{
	int		sockfd;
	int 	x = BORDER_HORIZ / 2, y = BORDER_VERT / 2, delta = 0;
	struct 	sockaddr_in	servaddr;
	char	sendline[MAXLINE], recvline[MAXLINE];
	char* 	type = "\x1b[0;3", *stop = "\x1b[0m", color[10], buff[10],
			res_str[50]; // for a beautiful output
	int 	color_num = getpid()%6 + 1;		
	
	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	
	sprintf( color, "%s%dm", type, color_num );
	sprintf( buff, "%d", getpid() );
	sprintf( res_str, "%s%d: x = %d, y = %d;%s\n", color, getpid(), x, 
			y, stop );
	if( fputs(res_str, stdout) == EOF ) perror( "fputs error\n" );
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_UNIX;
	servaddr.sin_port = htons(PORT);

	connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	// send pid, get delta, calculate new coords, if in borders - repeat
	while( ( 
		(x > 0) && (y > 0) && (x < BORDER_HORIZ) && (y < BORDER_VERT) 
		) ) {
		sprintf(sendline, "%d", getpid()); // pack client's pid into str
		write(sockfd, sendline, strlen(sendline)); // send pid to server
		if (read(sockfd, recvline, MAXLINE) == 0)  // wait answer
			perror("str_cli: server terminated prematurely");

		delta = atoi( recvline );
		x += delta;
		y += delta;
		sprintf( res_str, "%s%d: delta = %d, x = %d, y = %d;%s\n", 
				color, getpid(), delta, x, y, stop );
		if( fputs( res_str, stdout ) == EOF ) perror( "fputs error\n" );
	}
	
	if( close(sockfd) < 0 ) perror( "close error\n" );
	
	sprintf( res_str, "\x1b[1;3%dm%d finished with x = %d, y = %d\n", 
				color_num, getpid(), x, y );
	if( fputs( res_str, stdout ) == EOF ) perror( "fputs error\n" );
	exit(0);
}
