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
#define BUFF_SIZE 1024
#define PORT 60069

#define RED "\x1b[31m"
#define BLUE   "\x1B[34m"
#define YELLOW   "\x1B[33m"
#define RESET "\x1B[0m"

const char HEAD[] = "HTTP/1.1 200 OK		\n\
										Server: Hostname		\n\
										Content-Length: %ld		\
									\nConnection: close			\
									\nContent-Type: %s\n\n";

const char ERROR_HEAD[] = "HTTP/1.1 403 Forbidden\n\
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

	if(argc != 1) 
	{
		puts("Usage: ./httpserver\n");
		return ERROR;
	}

	memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(PORT);

	serverSockfd = socket(AF_INET, SOCK_STREAM, 0);
  bind(serverSockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	listen(serverSockfd, SOMAXCONN);

	printf("Server address: [%d]\n", serverAddr.sin_addr.s_addr);
	printf("Server port: [%d]\n", PORT);

	while(1)
	{
		clientSockfd = accept(serverSockfd, NULL, NULL);
		memset(request, 0, sizeof(request));

		read(clientSockfd, request, BUFF_SIZE);
		printf("\n%s\n", request);

		pid = fork();
		if(pid == 0)
		{
			char response[BUFF_SIZE];
			char *path;
			path = calloc(BUFF_SIZE, sizeof(char));

			if(checkHeader(request, path) == SUCCESS)
			{
				FILE *fp;
				int fileSize = getFileSize(path);

				printf("\nFilesize: [%d]\n", fileSize);
				if(fileSize != ERROR)
				{
					if(verifyExtension(path) == ERROR)
					{
						printf("ERROR: File is not of .html type.\n");
						return ERROR;
					}

					if ((fp = fopen(path, "r")) == NULL)
					{
						printf("ERROR: Failed to open file - [%s]\n", path);
						return ERROR;
					}

					int fd = fileno(fp), size;
					char buff[BUFF_SIZE];

					printf("   RESPONSE:   \n"
								 "===============\n");
					if(strstr(path, ".html"))
						sprintf(response, HEAD, (long)fileSize, "text/html");
					else if(strstr(path, ".jpg"))
						sprintf(response, HEAD, (long)fileSize, "image/jpeg");
					write(clientSockfd, response, strlen(response));

					if(fd < 0)
					{
						printf("\nERROR: main() - File size.\n");
						return ERROR;
					}
						
					while((size = read(fd, buff, BUFF_SIZE)) != 0) 
					{
						write(clientSockfd, buff, size);
						memset(buff, 0, strlen(buff));
					}
					fclose(fp);
				}
				else
					write(clientSockfd, ERROR_HEAD, strlen(ERROR_HEAD));
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

	if ((fp = fopen(path, "r")) == NULL)
	{
		printf("ERROR: Failed to open file - [%s]\n", path);
		return ERROR;
	}

	int fd = fileno(fp);
	if (fd < 0)
	{
		printf("\nERROR: getFileSize() - File size.\n");
		return ERROR;
	}

	while((count = read(fd, buff, BUFF_SIZE)) != 0) 
		result += count;

	fclose(fp);
	return result;
}

int verifyExtension(char *path)
{
	path++;
	while(*path != '\0' && *path != '.') 
		path++;

	printf("\nDEBUG: PATH = [%s]\n", path);
	if(strcmp(path, ".html") != 0 || strcmp(path, ".png") != 0)
		return ERROR;
	else
		return SUCCESS;
}