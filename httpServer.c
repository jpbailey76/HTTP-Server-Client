#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

// Defines 
#define ERROR -1
#define SUCCESS 0
#define BUFF_SIZE 1024
#define PORT 60069

#define RED "\x1b[31m"
#define BLUE   "\x1B[34m"
#define YELLOW   "\x1B[33m"
#define RESET "\x1B[0m"

char * filetypes[] = {
 	"image/gif", "image/jpeg","image/jpeg", "image/png", "image/zip",
  "image/gz", "image/tar", "text/html", "text/html"};

const char HEAD[] = "HTTP/1.0 200 OK\n\
										Server: Hostname\n\
										Content-Length: %ld\
										\nConnection: close\
										\nContent-Type: %s\n\n";

const char ERROR_HEAD[] = "HTTP/1.1 403 Forbidden\n\
													Server: Hostname\n\
													Content-Length:7\
													\nConnection: close\
													\nContent-Type: text\\html\n\nNO FILE";										

int main(int argc, char **argv)
{	
	int serverSockfd, clientSockfd, pid;
	struct sockaddr_in serverAddr;
	char request[BUFF_SIZE];

	if (argc != 1) 
	{
		puts("Usage: ./httpserver\n");
		return ERROR;
	}
	
	memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(PORT);

	serverSockfd = socket(AF_INET, SOCK_STREAM, 0);
  bind(serverSockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	listen(serverSockfd, SOMAXCONN);

	printf("Server address: [%d]\n", serverAddr.sin_addr.s_addr);
	printf("Server port: [%d]\n", PORT);

	while(1)
	{
		clientSockfd = accept(serverSockfd, NULL, NULL);
		memset(request, 0, sizeof(request));

		read(clientSockfd, request, BUFF_SIZE);
		write(1, request, strlen(request));
		write(1, "\n\n", sizeof("\n\n"));

		pid = fork();
		if(pid == 0)
		{
			char response[BUFF_SIZE];
			char *path;
			path = calloc(BUFF_SIZE, sizeof(char));

			if(checkHeader(request, path) == SUCCESS)
			{
				long fileSize;
				fileSize = getFileSize(path);
				
				if(fileSize != ERROR)
				{
					int type = getExtension(path);
					int fd = open(path, O_RDONLY);
					char buff[BUFF_SIZE];
					int readed;

					sprintf(response, HEAD, fileSize, filetypes[type]);
					write(clientSockfd, response, strlen(response));

					if (fd < 0)
						return ERROR;
					while((readed = read(fd, buff, BUFF_SIZE)) != 0) 
					{
						write(clientSockfd, buff, readed);
						memset(buff, 0, strlen(buff));
					}
					close(fd);
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
	char *pch;

	strcat(path,".");
  pch = strtok(header," ");
	pch = strtok (NULL, " ");
	strcat(path,pch);

	if(strcmp(path, "./") == 0)
		strcat(path, "index.html");

	pch = strtok (NULL, " \r\n");
	if(strcmp(pch, "HTTP/1.1") == 0 || strcmp(pch, "HTTP/1.0") == 0 )
		return SUCCESS;

	printf("checkHeader() - Invalid header.\n");
	return ERROR;
}

long getFileSize(char *path)
{
	int result = 0;
	int readed;

	char buff[BUFF_SIZE];

	int fd = open(path, O_RDONLY);
	if (fd < 0)
		return ERROR;

	while((readed = read(fd, buff, BUFF_SIZE)) != 0) 
	{
		result += readed;
	}

	return result;
}

int getExtension(char *path)
{
	path++;
	while(*path != '\0' && *path != '.') 
	{
		path++;
	}

	int i;
	for(i = 0; i<9; i++) 
	{
		if(strcmp(path, extensions[i]) == 0)
		{
			return i;
		}
	}

	return 8;
}