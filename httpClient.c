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
void printBinary(char*);

int main(int argc, char **argv)
{
  /* strings for user input
   * httpAddress = address of HTTP server to connect
   * fileRequest = name of file to request from server
   */
  char httpAddress[MAX_LENGTH];
  char fileRequest[MAX_LENGTH];
  
  /* server port */
  unsigned short httpPort;
  
  /* boolean to check if we're still connected to the server */
  // char isConnected = 0;
  
  /* socket declarations */
  int sockfd;
  struct sockaddr_in serverAddr;
  struct hostent *server;
  
  char invalid = 1;
  /* get HTTP address from user */
  while(invalid)
	{
  	printf("Enter address of HTTP server: ");
  	scanf("%s", httpAddress);

  	printf("Enter port on HTTP server: ");
  	scanf("%hu", &httpPort);

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
		int req;
	  char request[MAX_MESSAGE_LENGTH];
	  char response[MAX_MESSAGE_LENGTH];
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
		   exit(1);
		}
		
		/* test connection */
		if(connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
		{
		   printf("\nError: Connection to server failed!\n");
		   close(sockfd);
		   exit(1);
		}
		else
		{
		   printf("\nConnected to server.\n");
		}
		
		/* clear buffers */
		req = 0;
		bzero(fileRequest, MAX_LENGTH);
		bzero(request, MAX_MESSAGE_LENGTH);
		bzero(response, MAX_MESSAGE_LENGTH);
		
		/* ask which file to get */
		printf("\nWhat file do you want? ");
		scanf("%256s", fileRequest);

		sprintf(request, "GET /%s HTTP/1.1\r\n", fileRequest);
		strcat(request, "Connection: Keep-Alive\r\n\r\n");

		// Send the request
		req = write(sockfd, request, sizeof(request));
		if(req < 0)
		{
		   printf("\nError: Could not write to socket!\n");
		   exit(1);
		}
		else
		{
		   printf("\nRequest sent!\n");
		}
		printf("\nBUFFER2 = [%s]\n", request);

		// Read the response
		//req = read(sockfd, response, MAX_MESSAGE_LENGTH);
		printf("Server Response:\n");
		printf("================\n");
		while(read(sockfd, response, MAX_MESSAGE_LENGTH-1) != 0)
		{
		   printBinary((char*)response);
		   bzero(response, MAX_MESSAGE_LENGTH);
		}
		printf("\n================\n");

		close(sockfd);
	}
  
  return 0;
}

/* this functions prints out a buffer, character by character, on stdout
 *   as the client may receive non-textual binary data, this can be used
 *   to safely substitute non-printable characters without affecting the
 *   string or displaying garbage
 *
 * @ param 1 - string buffer to print
 */
void printBinary(char* buffer)
{
   int i;
   unsigned int length = strlen(buffer);
   for(i = 0; i < length; i++)
   {
      if((buffer[i] >= 32 && buffer[i] <= 126)
         || buffer[i] == '\n' || buffer[i] == '\t'
         || buffer[i] == '\r')
      {
         printf("%c", buffer[i]);
      }
      else printf(".");
   }
}
