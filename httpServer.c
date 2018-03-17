#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>

// Defines 
#define ERROR -1
#define SUCCESS 0
#define BUFF_SIZE 2048
#define PORT 60069

#define RED "\x1b[31m"
#define BLUE   "\x1B[34m"
#define YELLOW   "\x1B[33m"
#define RESET "\x1B[0m"

const char HEADER[] = "HTTP/1.1 200 OK		\n\
										Server: Hostname		\n\
										Content-Length: %ld		\
									\nConnection: close			\
									\nContent-Type: %s\n\n";

const char ERROR_HEADER[] = "HTTP/1.1 403 Forbidden\n\
													Server: Hostname\n 			 \
													Content-Length:7 				 \
												\nConnection: close				 \
												\nContent-Type: text\\html\n\nINVALID FILE";										

int checkHeader(char *header, char *path);
int getFileSize(char *path);
int verifyExtension(char *path);

int main(int argc, char **argv)
{	
	int serverSockfd, clientSockfd, pid;
	struct sockaddr_in serverAddr;
	char request[BUFF_SIZE];

	// Check that the program is being ran in a valid manner
	if(argc != 1) 
	{
		puts("Usage: ./httpserver\n");
		return ERROR;
	}

	// Create and bind the server socket
	memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(PORT);

	serverSockfd = socket(AF_INET, SOCK_STREAM, 0);
  bind(serverSockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	listen(serverSockfd, SOMAXCONN);

	printf("Server address: [%d]\n", serverAddr.sin_addr.s_addr);
	printf("Server port: [%d]\n", PORT);

	// Receive connections
	while(1)
	{
		// Accept the client 
		clientSockfd = accept(serverSockfd, NULL, NULL);
		memset(request, 0, sizeof(request));

		// Read their request
		read(clientSockfd, request, BUFF_SIZE);
		printf("\n%s\n", request);

		// Create a new process for responding
		pid = fork();
		if(pid == 0)
		{
			char response[BUFF_SIZE];
			char *path;
			path = calloc(BUFF_SIZE, sizeof(char));

			// Verify that we're receiving a valid request
			if(checkHeader(request, path) == SUCCESS)
			{
				FILE *fp;

				// Verify that the file requested exists
				int fileSize = getFileSize(path);

				printf("\nFilesize: [%d]\n", fileSize);
				if(fileSize != ERROR)
				{
					// Verify that the extension of the file is valid
					// We only handle html and jpg
					if(verifyExtension(path) == ERROR)
					{
						printf("ERROR: File is not of .html or .jpg type.\n");
						return ERROR;
					}

					// Open the requested file
					if ((fp = fopen(path, "r")) == NULL)
					{
						printf("ERROR: Failed to open file - [%s]\n", path);
						return ERROR;
					}

					// Get the file descriptor
					int filefd = fileno(fp), size;
					char buff[BUFF_SIZE];
					if(filefd < 0)
					{
						printf("\nERROR: main() - Invalid file descriptor.\n");
						return ERROR;
					}

					// Write our response to the client
					printf("   RESPONSE:   \n"
								 "===============\n");
					if(strstr(path, ".html") != NULL)
						sprintf(response, HEADER, (long)fileSize, "text/html");
					else if(strstr(path, ".jpg") != NULL)
						sprintf(response, HEADER, (long)fileSize, "image/jpeg");
					write(clientSockfd, response, strlen(response));

					while((size = read(filefd, buff, BUFF_SIZE)) != 0) 
					{
						write(clientSockfd, buff, size);
						printf("\nSending file data =============\n%s\n=============\n", buff);
						memset(buff, 0, strlen(buff));
					}
					fclose(fp);
				}
				else	// They sent us an invalid request, so we send them the error header.
					write(clientSockfd, ERROR_HEADER, strlen(ERROR_HEADER));
			}
			free(path);
			close(clientSockfd);
			close(serverSockfd);
			return SUCCESS;
		}
		else 
			close(clientSockfd);			
	}

	return SUCCESS;
}

int checkHeader(char *header, char *path)
{
	char *requestType, *file, *protocol;

	// Parse
	strcat(path,".");
  requestType = strtok(header," ");
	file = strtok (NULL, " ");
	strcat(path, file);
	if(strcmp(path, "./") == 0)
		strcat(path, "index.html");
	protocol = strtok (NULL, " \r\n");

	// Display
	printf("Request Type: [%s]\n", requestType);
	printf("File: [%s]\n", file);
	printf("protocol: [%s]\n", protocol);

	// Verify
	if(strcmp(protocol, "HTTP/1.1") == 0)
		return SUCCESS;

	printf("ERROR: checkHeader() - Invalid header.\n");
	return ERROR;
}

int getFileSize(char *path)
{
	int result = 0, count;
	char buff[BUFF_SIZE];
	FILE *fp;

	// Open file at path
	if ((fp = fopen(path, "r")) == NULL)
	{
		printf("ERROR: Failed to open file - [%s]\n", path);
		return ERROR;
	}

	// Get the file descriptor
	int fd = fileno(fp);
	if (fd < 0)
	{
		printf("\nERROR: getFileSize() - File size.\n");
		return ERROR;
	}

	// Count the size
	while((count = read(fd, buff, BUFF_SIZE)) != 0) 
		result += count;

	fclose(fp);
	return result;
}

int verifyExtension(char *path)
{
	// Parse the path until you reach the file extension
	path++;
	while(*path != '\0' && *path != '.') 
		path++;

	// Check if valid
	if( (strcmp(path, ".html") == 0) 
		||(strcmp(path, ".jpg")  == 0) )
		return SUCCESS;
	else
		return ERROR;
}