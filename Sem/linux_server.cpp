/* The port number is passed as an argument */
#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <ctime>
#include <string>
#include <iostream>
#include <stdio.h>

using namespace std;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno;
    char buffer[4096];
    char *newl = "\n";
    char *ipa = "IP Address:  ";
    struct sockaddr_in serv_addr, cli_addr;
    int n; 
	socklen_t clilen;
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    // create a socket
    // socket(int domain, int type, int protocol)
    sockfd =  socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    	error("ERROR opening socket");

    // clear address structure
    bzero((char *) &serv_addr, sizeof(serv_addr));

    portno = atoi(argv[1]);

    /* setup the host_addr structure for use in bind call */
    // server byte order
    serv_addr.sin_family = AF_INET;  

    // automatically be filled with current host's IP address
    serv_addr.sin_addr.s_addr = INADDR_ANY;  

    // convert short integer value for port must be converted into network byte order
    serv_addr.sin_port = htons(portno);

    // bind(int fd, struct sockaddr *local_addr, socklen_t addr_length)
    // bind() passes file descriptor, the address structure, 
    // and the length of the address structure
    // This bind() call will bind  the socket to the current IP address on port, portno
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) error("ERROR on binding");

    // This listen() call tells the socket to listen to the incoming connections.
    // The listen() function places all incoming connection into a backlog queue
    // until accept() call accepts the connection.
    // Here, we set the maximum size for the backlog queue to 5.
    listen(sockfd,5);

	while(1)
	{
    	// The accept() call actually accepts an incoming connection
    	clilen = sizeof(clilen);

    	// This accept() function will write the connecting client's address info 
     	// into the the address structure and the size of that structure is clilen.
     	// The accept() returns a new socket file descriptor for the accepted connection.
     	// So, the original socket file descriptor can continue to be used 
     	// for accepting new connections while the new socker file descriptor is used for
     	// communicating with the connected client.
     	if ((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) == -1) {
        	error("ERROR on accept");
		}


		/////////////////////
		FILE *file_ptr;
		file_ptr = fopen("connections.txt", "a");    // Saves all connections to .txt file!"
		{
			time_t timer;
			timer=time(NULL);

			fputs(ipa, file_ptr);
			fputs(inet_ntoa(cli_addr.sin_addr), file_ptr);
			fputs(newl, file_ptr);
			fputs(asctime(localtime(&timer)), file_ptr);
			fputs(newl, file_ptr);
			fclose(file_ptr);
		}
		
		printf("Incoming connection from: %s \n", inet_ntoa(cli_addr.sin_addr) );

		time_t timer;
		timer=time(NULL);
		printf("Time connected: %s \n", asctime(localtime(&timer)) ); // Prints the time the connected
		/////////////////////

     	printf("server: got connection from %s port %d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));


     	// This send() function sends the 13 bytes of the string to the new socket
     	send(newsockfd, "Data recieved by remote server\n", 32, 0);

     	bzero(buffer,4096);

     	n = read(newsockfd,buffer,4095);
     	if (n < 0) error("ERROR reading from socket");
     	
     	//printf("Here is the message: %s\n",buffer); //prints recieved content

		std::time_t rawtime;
		std::time ( &rawtime);
		const std::tm* timeinfo = std::localtime ( &rawtime );
		
		char yyyymmdd[32];
		//std::string actualTime;
		std::strftime(yyyymmdd, sizeof(yyyymmdd), "%Y-%m-%d_%H-%M-%S", timeinfo);
		
		//printf(yyyymmdd);
			
		FILE *logfile;
		logfile = fopen(yyyymmdd, "w");    // Saves recieved buffer .txt file!"
		fprintf(logfile, "%s\n", buffer);
		fclose(logfile);


     	close(newsockfd);
     	//close(sockfd);
	}
	//return 0; 
}

