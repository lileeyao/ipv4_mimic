// @file	:tcpclient.c
// @Author 	:Linyao Li 1206374165
// @Date 	:2014-11-01

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

// Set buffer size = 257 bytes.
#define BufferLength 257 

#define SERVER "the_server_name_or_ip"
//server port num
#define SERVPORT 3111

int main(int argc, char *argv[])
{
	int 	sd, rc, length = sizeof(int);
	struct 	sockaddr_in serveraddr;
	char 	buffer[BufferLength];
	char 	server[255];
	char 	temp;
	int	totalcnt = 0;
	struct	hostent *hostp;
	char	data[100] = "Test string from client";

	//get a socket descriptor
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("client-socket() error");
		exit(-1);		
	}
	else
		printf("client-socket() OK\n");
	if (argc > 1)
	{
		strcpy(server, argv[1]);
		printf("connecting to the %s, port %d ... \n", server, SERVPORT);	
	}
	else
		strcpy(server, SERVER);
	memset(&serveraddr, 0x00, sizeof(struct sockaddr_in));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port   = htons(SERVPORT);
	if ((serveraddr.sin_addr.s_addr = inet_addr(server)) == (unsigned long)INADDR_NONE) {
	// get host addr
	hostp = gethostbyname(server);
	if (hostp == (struct hostent *) NULL) {
		printf("host not found -->");
		printf("h_errno = %d\n", h_errno);
		printf("CLIENT PROGRAM\n");
		printf("command usage: %s <server name or ip>", argv[0]);
		close(sd);
		exit(-1);
	}
	memcpy(&serveraddr.sin_addr, hostp->h_addr, sizeof(serveraddr.sin_addr));
}
	// connect to server.
	if ((rc = connect(sd, (struct sockaddr *)&serveraddr, sizeof(serveraddr))) < 0) {
	perror("client-connect() error");
	close(sd);
	exit(-1);
	}
	else 
		printf("connection established..\n");
	//write to buffer
	printf("sending some string to %s...\n", server);
	rc = write(sd, data, sizeof(data));
	
	if (rc < 0) {
		perror("client-write() error");
		rc = getsockopt(sd, SOL_SOCKET, SO_ERROR, &temp, &length);
		if(rc == 0) {
			errno = temp;
			perror("SO_ERROR wa");
		}
		close(sd);
		exit(-1);
	}
	else {
		printf("client-write is ok\n");
		printf("string successfully sent...ok\n");
		printf("waiting the %s to echo back...ok\n", server);
	}
	totalcnt = 0;
	while (totalcnt < BufferLength) {
		rc = read(sd, &buffer[totalcnt], BufferLength-totalcnt);
		if (rc < 0) {
			perror("client-read error");
			close(sd);
			exit(-1);
		}
		else if (rc == 0) {
			printf("server has issed a close()\n");
			close(sd);
			exit(-1);
		}
		else
			totalcnt += rc;
	}

	printf("client-read is ok..\n");
	printf("echoed data from server: %s \n", buffer);
	//close socket descriptor from client side.
	close(sd);
	exit(0);
	return 0;
}


