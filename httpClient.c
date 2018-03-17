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
  char isConnected = 0;
  
  /* socket declarations */
  int sockfd;
  struct sockaddr_in serverAddr;
  struct hostent *server;
  
  /* get HTTP address from user */
  printf("Hello! I'm a talking string!\n");
  
  printf("Enter address of HTTP server: ");
  scanf("%256s", httpAddress);

  printf("\nYou entered the address [%s]\n", httpAddress);
  printf("String length is %d\n", (int)strlen((const char*)httpAddress));
  
  printf("Enter port on HTTP server: ");
  scanf("%hu", httpPort);
  
  printf("\nYou entered the port %d\n", *httpPort);
  server = gethostbyname((char*)httpAddress);
  
  /* check if the address is valid */
  if(server == NULL)
  {
     printf("\nError! No such host at \"%s\"\n", httpAddress);
     exit(1);
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
	char *buffer = (char*)calloc(MAX_MESSAGE_LENGTH, sizeof(char));
	printf("\nWhat file do you want? ");
	scanf("%256s", fileRequest);
	
	bzero(buffer, MAX_MESSAGE_LENGTH);
	sprintf(buffer, "GET %s HTTP/1.1\r\n", fileRequest);
	strcat(buffer, "Connection: Keep-Alive\r\n\r\n");

	printf("\nBUFFER1 = [%s]\n", buffer);
	printf("DEBUG1\n");
	
	req = write(sockfd, buffer, strlen(buffer));
	if(req < 0)
	{
	   printf("\nError! Could not write to socket!\n");
	   exit(1);
	}
	else
	{
			printf("DEBUG2\n");
	   while(write(sockfd, buffer, sizeof(buffer)) > 0);
			printf("DEBUG3\n");
	}
	
	printf("BUFFER2 = [%s]\n", buffer);

	printf("DEBUG4\n");


	bzero(buffer, MAX_MESSAGE_LENGTH);
	
	req = read(sockfd, buffer, MAX_MESSAGE_LENGTH);
	if(req < 0)
	{
	   printf("\nError! Could not read from socket!\n");
	   exit(1);
	}
	else
	{
	   while(read(sockfd, buffer, sizeof(buffer)) > 0);
	}
	
	printf("\n\n\n\n\n");
	
	printf("Buffer: [%s]\n", buffer);
	bzero(buffer, MAX_MESSAGE_LENGTH);
	bzero(fileRequest, MAX_LENGTH);
	printf("Buffer is now [%s]\n", buffer);
	printf("Req is now [%s]\n", fileRequest);
  printf("\nWhat file do you want? ");
	scanf("%256s", fileRequest);
	
	bzero(buffer, MAX_MESSAGE_LENGTH);
	sprintf(buffer, "GET %s HTTP/1.1\r\n", fileRequest);
	req = write(sockfd, buffer, strlen(buffer));
	if(req < 0)
	{
	   printf("\nError! Could not write to socket!\n");
	   exit(1);
	}
	else
	{
	   while(write(sockfd, buffer, sizeof(buffer)) > 0);
	}
	
	bzero(buffer, MAX_MESSAGE_LENGTH);
	
	req = read(sockfd, buffer, MAX_MESSAGE_LENGTH);
	if(req < 0)
	{
	   printf("\nError! Could not read from socket!\n");
	   exit(1);
	}
	else
	{
	   while(read(sockfd, buffer, sizeof(buffer)) > 0);
	}
  
  printf("Buffer: [%s]\n", buffer);
  
  
  
  
  
  
  
  
  
  
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
