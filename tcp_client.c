/*
    MIDHUN NAIR
    
    Updated Date:   07/01/2021

    TCP-CLIENT IMPLEMENTATION

*/

#include <stdio.h>
#include <sys/types.h>      
#include <sys/socket.h>     
#include <netinet/in.h>    
#include <netinet/ip.h>    
#include <unistd.h>         
#include <stdlib.h>         
#include <strings.h>      
#include <string.h>    
#include <arpa/inet.h>  
#include<signal.h>

#define BUFFER_SIZE 1024

void SIGINT_HANDLER(int);  

char sendbuffer[1024];    
char receivebuffer[1024];

int sockfd;

int main(int argc, char *argv[])
{
    int portnumber;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];

    if(argc < 2)
    {
        printf("Enter the port number !! \n");
        printf("Client App is shutting Down\n");
        exit(0);
    }
   
    sockfd = socket(AF_INET,SOCK_STREAM,0);  /*Creating a socket file descriptor*/
    if (sockfd < 0)
    {
        printf("Failed to create socket \n");
	    exit(EXIT_FAILURE);
    }
    else
    {
        printf("Socket is created Successfully\n");
    }
    
    bzero(&serv_addr,sizeof(serv_addr));
    portnumber = atoi(argv[1]); 

    /*Initializing server address*/

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr =  inet_addr("127.0.0.1");  
    serv_addr.sin_port =  htons(portnumber);
    
    int retval = connect(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr)); 
    if (retval < 0)
        printf("Connection to server is failed \n");
    else
        printf("Connected to the server successfully\n\n");
  

    /*************SENDER-RECEIVER*********************/

    pid_t retf = fork();

    if(retf < 0)
    {
    	perror("");
	exit(EXIT_FAILURE);
    }

    /****SENDING TO SERVER****/

    if(retf > 0)      
    {
    	while(1)
	{
        	signal(SIGINT, SIGINT_HANDLER);
	    	bzero(&sendbuffer, sizeof(sendbuffer));
		fgets(sendbuffer, sizeof(sendbuffer), stdin);
		size_t sret = send(sockfd, sendbuffer, strlen(sendbuffer)+1, 0);
		if(sret < 0)
		{
			printf("\nsending failed..Try Again\n");
		}
	}
    }

    /****RECEIVING FROM SERVER****/

    if(retf == 0)    
    {
    	while(1)
   	{
    		bzero(&receivebuffer, sizeof(receivebuffer));
    		size_t rret = recv(sockfd, receivebuffer, sizeof(receivebuffer), 0);
    		if(!strcmp(receivebuffer, "\nServer Disconnected\n"))
		{
			printf("Server Lost Connection..\n");
			exit(0);
		}
      		if((signed long int)rret > 0)
         		printf("Msg from Server: %s\n", receivebuffer);
     	}
    }
 
    close(sockfd);
    return 0;
} 

/* SIGNAL HANDLER IF USER PRESS CTRL+C */

void SIGINT_HANDLER(int signum)
{
    strcpy(sendbuffer,"\nClient lost connection\n");
    send(sockfd, sendbuffer, strlen(sendbuffer)+1, 0);
    printf("\nDisconnected Client\n");
    close(sockfd);
    exit(EXIT_FAILURE);
}
