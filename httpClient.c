#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>

/* consts and definitions */
#define MAX_LENGTH 256
#define MAX_MESSAGE_LENGTH 2048

/* prototypes */
void requestFile();
unsigned char isConnected();

int main(int argc, char **argv)
{
  /* strings for user input
   * httpAddress = address of HTTP server to connect
   * fileRequest = name of file to request from server
   */
  char httpAddress[MAX_LENGTH];
  char fileRequest[MAX_LENGTH];
  
  /* server port */
  int httpPort;
  
  /* boolean to check if we're still connected to the server */
  // char isConnected = 0;
  
  /* socket declarations */
  int sockfd;
  struct sockaddr_in serverAddr;
  struct hostent *server;
  
  int invalid = 1;
  /* get HTTP address from user */
  while(invalid)
	{
  	printf("Enter address of HTTP server: ");
  	scanf("%s", httpAddress);

  	printf("Enter port on HTTP server: ");
  	scanf("%d", &httpPort);

	  /* check if the address is valid */
  	server = gethostbyname((char*)httpAddress);
	  if(server == NULL)
	  {
	     printf("\nError! No such host at \"%s\"\n", httpAddress);
	     invalid = 1;
		}
  	else
  		invalid = 0;
  }
	
	invalid = 1;
	while(invalid)
	{
		/* create the client socket */
		memset(&serverAddr, 0, sizeof(serverAddr));
		bcopy((char*)server->h_addr,(char*)&serverAddr.sin_addr.s_addr,
		       server->h_length);
	  serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(httpPort);
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		
		/* check the socket */
		if(sockfd < 0)
		{
		   printf("\nError! Could not open socket\n");
		   close(sockfd);
		   continue;
		}
		
		/* test connection */
		if(connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
		{
		   printf("\nError: Connection to server failed!\n");
		   close(sockfd);
		   continue;
		}
		else
		{
		   printf("\nConnected to server.\n");
		}

		int req;
		char request[MAX_MESSAGE_LENGTH];
		char response[MAX_MESSAGE_LENGTH];
		printf("\nWhat file do you want? ");
		scanf("%256s", fileRequest);

		memset(request, 0, sizeof(request));
		sprintf(request, "GET /%s HTTP/1.1\r\n", fileRequest);
		strcat(request, "Connection: Keep-Alive\r\n\r\n");

		// Send the request
		req = write(sockfd, request, sizeof(request));
		if(req < 0)
		{
		   printf("\nError: Could not write to socket!\n");
		   continue;
		}
		else
		{
		   printf("\nRequest sent!\n");
		}
		printf("\nBUFFER2 = [%s]\n", request);
		bzero(request, MAX_MESSAGE_LENGTH);

		// Read the response
		req = read(sockfd, response, MAX_MESSAGE_LENGTH);
		if(req < 0)
		{
		   printf("\nError! Could not read from socket!\n");
		   exit(1);
		}
		else
		{
			printf("\nReceiving response.\n");
		   // while(read(sockfd, request, sizeof(request)) > 0);
		}
		printf("Server Response:\n");
		printf("================\n");
		printf("%s", response);
		close(sockfd);
	}
  
  return 0;
}

void requestFile()
{
}

unsigned char isConnected();
