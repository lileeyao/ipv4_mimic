//Disclaimer: This source file is based on the implementation of Mark Yates.
//
//Register the protocol and device driver module
//Create functions to send message to the destination VM using IPv4
//Write read and write method for the decive driver
//Register the protocol number
//Create receive function using IPv4.
/*
* @file    cse536app.c
* @Author  Linyao Li
* @Date    2014-11-02
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 256

main(int argc, char *argv[]) {
		FILE *fd = NULL;
		char buffer[BUFFER_SIZE + 1], data[BUFFER_SIZE];
		char *remote_ip, input;
		size_t count;
		int end = 0, ch;
		remote_ip = "192.168.169.135";
		memset(buffer, 0, BUFFER_SIZE + 1);
		buffer[0] = 1;
		memcpy(buffer + 1, remote_ip, strlen(remote_ip) + 1);

		fd = fopen("/dev/cse5361", "wb");
		if (!fd) {
			printf("File Error Opening !\n");
			exit(1);
		}
		fwrite(buffer, 1, 30, fd);
		fclose(fd);

		while (end == 0) {
			printf("S) Send the message to ip:192.168.169.135\n");
			printf("R) Read the message from ip:192.168.169.134\n");
			printf("E) End\n");
			scanf("%c", &input);
			input = toupper(input);
			// clear the standard input stream
			while ((ch = fgetc(stdin)) != EOF && ch != '\n' ) {
				switch (input) {
						case 'S':
							printf("Select mode $S/R/E$:\n");
							scanf("%256s", data);
							while ((ch = fgetc(stdin)) != EOF && ch != '\n' ) {
								memset(buffer, 0, BUFFER_SIZE + 1);
								buffer[0] = 2;
								memcpy(buffer + 1, data, strlen(data) + 1);
								fd = fopen("/dev/cse5361", "wb");
								if (fd) {
										fwrite(buffer, 1, strlen(data) + 1, fd);
										printf("Message sent: %s\n", data);
										fclose(fd);
								}
							}
							break;
						case 'R':
							fd = fopen("/dev/cse5361", "rb");
							if (fd) {
								count = fread(data, 1, sizeof(data) - 1, fd);
								if (!count) printf("No data found!\n");
								else printf("%s\n", data);
								fclose(fd);
							}
								break;
						case 'E':
							end = 1;
								break;
						default :
							printf("Invalid!\n");
							break;
						}
					}
}
