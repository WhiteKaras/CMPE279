/* 
He Fu
012543440
Rui Guo
012478973
Assignment 3
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
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>

int main(int argc, char const *argv[]) 
{ 
    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1, port = 8080; 
    int addrlen = sizeof(address); 
    char buffer[1024] = {0}; 
    char *hello = "Hello from server"; 
	pid_t fpid = 0;
	int fd = 0;
	
	// Original
	if (argc == 1 || (strcmp(argv[1], "-c") != 0) ){
		printf("Parent start\n");
		
		char *char_port = "8080";
		char FileName[FILENAME_MAX];
		char char_fd[100];
		int file_flag = 0;
			
		// Set up port and filename if needed
		for (int i = 1; i < argc - 1; i += 2){
			if (strcmp(argv[i], "-p") == 0) {
				strcpy(char_port, argv[i+1]);
				port = atoi(argv[i+1]);
			}
			if (strcmp(argv[i], "-f") == 0){
				strcpy(FileName, argv[i+1]);
				file_flag = 1;
			}
		}

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
		address.sin_port = htons( port ); 
		   
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

		// Read msg file if needed
		if (file_flag)
		{
			fd = open(FileName, O_RDONLY|O_NONBLOCK);
			sprintf (char_fd, "%d", fd);
        }
	
		// Fork for privilege separation
		printf("Fork\n");
		pid_t fpid = fork();

		
		// Re-exec child
		if (fpid == 0){
			char char_socket[12];
			sprintf(char_socket, "%d", new_socket);
			
			if (file_flag){
				char *args[] = {"./server", "-c", char_socket, char_port, char_fd, NULL};
				printf("Exec\n");
				execvp(args[0], args);
			}
			else{
				char *args[] = {"./server", "-c", char_socket, char_port, NULL};
				printf("Exec\n");
				execvp(args[0], args);
			}
		}
		// Parent job
		else{
			int status = 0;
			while ((wait(&status) > 0));
			printf("Parent done\n");
			
			close(fd);
		}
		
	}
	//Child job
	else{	
		printf("Child start\n");
		char *text = (char *) calloc(1024, sizeof(char));
		
		// Read file if needed
		if (argc == 5){
			fd = atoi(argv[4]);
			read(fd, text, 1024);
		}
		else{
			strcpy(text, hello);
		}

		// Chroot to empty folder "jail"
		char cwd[FILENAME_MAX];
		getcwd(cwd, sizeof(cwd));
		chdir(strcat(cwd, "/jail"));
		chroot(strcat(cwd, "/jail"));
		printf("Chroot done\n");
		
		// Drop privilege to nobody
		setuid(65534);
		printf("Privilege dropped\n\n");
		
		new_socket = atoi(argv[2]);

		valread = read( new_socket , buffer, 1024); 
		printf("%s\n",buffer ); 
		send(new_socket , text , strlen(text) , 0 ); 
		printf("Hello message sent\n"); 
		
		printf("\nChild done\n");
	}
	
    return 0; 
} 
