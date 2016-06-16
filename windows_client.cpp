// client.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <winsock.h>

#include <iostream>
#include <conio.h>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <algorithm>

#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
#define bcopy(b1,b2,len) (memmove((b2), (b1), (len)), (void) 0)

#pragma warning(disable:4996)
#pragma comment(lib, "Ws2_32.lib")



void error(const char *msg)
{
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[])
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(1, 1), &wsaData);

	/////////////////////////
	FILE *fp, *outputfile;
	//char var[131072];
	//char info_buffer[131072];
	char var[4096];
	char info_buffer[4096];
	int bufSize;

	strcpy(info_buffer, "");

	fp = _popen("systeminfo", "r");
	while (fgets(var, sizeof(var), fp) != NULL)
	{
		strcat(info_buffer, var);
	}
	_pclose(fp);

	//printf(info_buffer); //prints system info on screen

	bufSize = sizeof(info_buffer);

	printf("%d", bufSize);

	outputfile = fopen("text.txt", "a");
	fprintf(outputfile, "%s\n", info_buffer);
	fclose(outputfile);
	/////////////////////////


	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	//char buffer[256];
	//char buffer[131072];
	char buffer[4096];
	if (argc < 3) {
		fprintf(stderr, "usage %s hostname port\n", argv[0]);
		exit(0);
	}
	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");
	server = gethostbyname(argv[1]);
	if (server == NULL) {
		fprintf(stderr, "ERROR, no such host\n");
		exit(0);
	}
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,
		(char *)&serv_addr.sin_addr.s_addr,
		server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR connecting");
	printf("Please enter the message: ");
	bzero(buffer, 256);
	fgets(buffer, 255, stdin);
	//n = send(sockfd, buffer, strlen(buffer), 0);
	n = send(sockfd, info_buffer, strlen(info_buffer), 0);

	if (n < 0)
		error("ERROR writing to socket");
	bzero(buffer, 256);
	n = recv(sockfd, buffer, 255, 0);
	if (n < 0)
		error("ERROR reading from socket");
	printf("%s\n", buffer);
	closesocket(sockfd);
	return 0;
}


