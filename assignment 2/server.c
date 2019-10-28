/* 
He Fu
012543440
Rui Guo
012478973
Assignment 2
CMPE 279

Team member: He Fu / Rui Guo
*/

// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#define PORT 8080 
int main(int argc, char const *argv[]) 
{ 
    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    char buffer[1024] = {0}; 
    char *hello = "Hello from server"; 
	pid_t fpid = 0;
	
	// Original
	if (argc == 1){
		printf("Parent start\n");
		
		// Creating socket file descriptor 
		if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
		{ 
			perror("socket failed"); 
			exit(EXIT_FAILURE); 
		} 
		   
		// Forcefully attaching socket to the port 8080 
		if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
													  &opt, sizeof(opt))) 
		{ 
			perror("setsockopt"); 
			exit(EXIT_FAILURE); 
		} 
		address.sin_family = AF_INET; 
		address.sin_addr.s_addr = INADDR_ANY; 
		address.sin_port = htons( PORT ); 
		   
		// Forcefully attaching socket to the port 8080 
		if (bind(server_fd, (struct sockaddr *)&address,  
									 sizeof(address))<0) 
		{ 
			perror("bind failed"); 
			exit(EXIT_FAILURE); 
		} 
		if (listen(server_fd, 3) < 0) 
		{ 
			perror("listen"); 
			exit(EXIT_FAILURE); 
		} 
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
						   (socklen_t*)&addrlen))<0) 
		{ 
			perror("accept"); 
			exit(EXIT_FAILURE); 
		} 
		
		// Fork for privilege separation
		printf("Fork\n");
		pid_t fpid = fork();
		
		// Re-exec child
		if (fpid == 0){
			char char_socket[12];
			sprintf(char_socket, "%d", new_socket);
			char *args[] = {"./server", char_socket, NULL};
			printf("Exec\n");
			execvp(args[0], args);
		}
		// Parent job
		else{
			int status = 0;
			while ((wait(&status) > 0));
			printf("Parent done\n");
		}
		
	}
	//Child job
	else{
		new_socket = atoi(argv[1]);
		
		printf("Child start\n");
		// Drop privilege to nobody
		setuid(65534);
		printf("Privilege dropped\n\n");

		new_socket = atoi(argv[1]);
		valread = read( new_socket , buffer, 1024); 
		printf("%s\n",buffer ); 
		send(new_socket , hello , strlen(hello) , 0 ); 
		printf("Hello message sent\n"); 
		
		printf("\nChild done\n");
	}
	
    return 0; 
} 
