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
  char* httpAddress = (char*)calloc(MAX_LENGTH, sizeof(char));
  char* fileRequest = (char*)calloc(MAX_LENGTH, sizeof(char));
  
  /* server port */
  unsigned short* httpPort = (unsigned short*)calloc(1, sizeof(short));
  
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
  	scanf("%256s", httpAddress);

  	printf("Enter port on HTTP server: ");
  	scanf("%hu", httpPort);

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

  /* create the client socket */
	memset(&serverAddr, 0, sizeof(serverAddr));
	bcopy((char*)server->h_addr,(char*)&serverAddr.sin_addr.s_addr,
	       server->h_length);
  serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(*httpPort);
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	/* check the socket */
	if(sockfd < 0)
	{
	   printf("\nError! Could not open socket\n");
	   exit(1);
	}
	
	/* test connection */
	if(connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{
	   printf("\nError! Connection to server failed!\n");
	   return 0;
	}
	else
	{
	   printf("\nHey! We're connected!!!\n");
	}
	
	int req;
	char request[MAX_MESSAGE_LENGTH];
	printf("\nWhat file do you want? ");
	scanf("%256s", fileRequest);

  memset(request, 0, sizeof(request));
	sprintf(request, "GET %s HTTP/1.1\r\n", fileRequest);
	strcat(request, "Connection: Keep-Alive\r\n\r\n");
	
	req = write(sockfd, request, strlen(request));
	if(req < 0)
	{
	   printf("\nError! Could not write to socket!\n");
	   exit(1);
	}
	else
	{
	   while(write(sockfd, request, sizeof(request)) > 0);
	}
	printf("\nBUFFER2 = [%s]\n", request);

	bzero(request, MAX_MESSAGE_LENGTH);
	
	req = read(sockfd, request, MAX_MESSAGE_LENGTH);
	if(req < 0)
	{
	   printf("\nError! Could not read from socket!\n");
	   exit(1);
	}
	else
	{
	   while(read(sockfd, request, sizeof(request)) > 0);
	}
	
	printf("\n\n\n\n\n");
	
	printf("Buffer: [%s]\n", request);
	bzero(request, MAX_MESSAGE_LENGTH);
	bzero(fileRequest, MAX_LENGTH);
	printf("Buffer is now [%s]\n", request);
	printf("Req is now [%s]\n", fileRequest);
  printf("\nWhat file do you want? ");
	scanf("%256s", fileRequest);
	
	bzero(request, MAX_MESSAGE_LENGTH);
	sprintf(request, "GET %s HTTP/1.1\r\n", fileRequest);
	req = write(sockfd, request, strlen(request));
	if(req < 0)
	{
	   printf("\nError! Could not write to socket!\n");
	   exit(1);
	}
	else
	{
	   while(write(sockfd, request, sizeof(request)) > 0);
	}
	
	bzero(request, MAX_MESSAGE_LENGTH);
	
	req = read(sockfd, request, MAX_MESSAGE_LENGTH);
	if(req < 0)
	{
	   printf("\nError! Could not read from socket!\n");
	   exit(1);
	}
	else
	{
	   while(read(sockfd, request, sizeof(request)) > 0);
	}
  
  printf("Buffer: [%s]\n", request);
  
  
  
  
  
  
  
  
  
  
  /* while we're connected, allow the user to keep requesting for additional
     files */
     
     /*
  while(isConnected())
  {
     requestFile();
  }
  return 0;
  */
  
  return 0;
}

void requestFile()
{
}

unsigned char isConnected();
