// @file:	tcpserver.c
// @Author:	Linyao Li 1206374165
// @Date:	2014-11-01

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BufferLength 257

#define SERVPORT 3111

int main() {
	int 	sd, sd2, rc, length = sizeof(int);
	int 	totalcnt = 0, on = 1;
	char	temp;
	char	buffer[BufferLength];
	struct	sockaddr_in serveraddr;
	struct	sockaddr_in their_addr;
	fd_set	read_fd;
	struct	timeval timeout;
	timeout.tv_sec = 15;
	timeout.tv_usec = 0;
	
	// get a socket descriptor
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("server-socket() error");
		exit(-1);

	}
	else 
		printf("server-socket() is ok\n");
	
	//allow socket descriptor to be reusable
	if ((rc = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)))< 0) {
		perror("server-sersockopt() error");
		close(sd);
		exit(-1);
	}
	else {
		printf("server-setsockopt() is ok\n");
	}
	printf("server-setsockopt() is ok\n");
	memset(&serveraddr, 0x00, sizeof(struct sockaddr_in));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVPORT);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	printf("Using %s, listening  at %d\n", inet_ntoa(serveraddr.sin_addr), SERVPORT);
	
	//connect to client of same port.

	if ((rc = bind(sd, (struct sockaddr *)&serveraddr, sizeof(serveraddr))) < 0) {
	perror("server-bind() error");
	close(sd);
	exit(-1);
	}
	else
		printf("server-bind() is ok\n");

	//up to 10 clients can be queued
	if ((rc = listen(sd, 10)) < 0) {
		perror("server-listen() error");
		close(sd);
		exit(-1);
	}
	else
		printf("server-ready for client connection...\n");
	//accept incoming connection request
	int sin_size = sizeof(struct sockaddr_in);
	if ((sd2 = accept(sd, (struct sockaddr *)&their_addr, &sin_size)) < 0)
	{
		perror("server-accept() error");
	close(sd);
	exit(-1);	
	}
	else
		printf("server-accept is ok...\n");
	//client ip
	printf("server-new socket, sd2 is ok...\n");
	printf("got connection from the client:%s\n", inet_ntoa(their_addr.sin_addr));
	//select for data to be read
	FD_ZERO(&read_fd);
	FD_SET(sd2, &read_fd);
	rc = select(sd2+1, &read_fd, NULL, NULL, &timeout);
	if ((rc == 1) && (FD_ISSET(sd2, &read_fd))){
	totalcnt = 0;
	while(totalcnt < BufferLength) {
		//read
	rc = read(sd2, &buffer[totalcnt], (BufferLength - totalcnt));
	if(rc < 0) {
		perror("server-read() error");
		close(sd);
		close(sd2);
		exit(-1);
	}
	else if(rc == 0) {
		printf("client program has issued a close()\n");
		close(sd);
		close(sd2);
		exit(-1);
	}
	else {
		totalcnt += rc;
		printf("server-read() is ok\n");

	}
	}
	}
	else if(rc < 0) {
		perror("server-select() error");
		close(sd);
		close(sd2);
		exit(-1);
	}
	else {
		printf("server-select() time out.\n");
		close(sd);
		close(sd2);
		exit(-1);
	}

	//show final data
	printf("received data from client: %s\n", buffer);

	//back to client
	printf("server-Echoing back to client...\n");
	rc = write(sd2, buffer, totalcnt);
	if (rc != totalcnt) {
		perror("Server-write() error");
		rc = getsockopt(sd2, SOL_SOCKET, SO_ERROR, &temp, &length);
		if(rc == 0) {
			errno = temp;
			perror("SO_ERROR was:");
		}
		else	
			printf("server-write() is ok\n");
		close(sd);
		close(sd2);
		exit(-1);
	}

	close(sd2);
	close(sd);
	exit(0);
	return 0;
}
