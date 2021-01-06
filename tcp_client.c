/*
  Midhun Nair 
  Updated Date: 6/1/2021 
  TCP-CLIENT
*/

#include <stdio.h>
#include <sys/types.h>       //socket/connect 
#include <sys/socket.h>     //socket/connect
#include <netinet/in.h>    // struct sockaddr_in (man 7 ip)
#include <netinet/ip.h>    // struct sockaddr_in (man 7 ip)
#include <unistd.h>         //read and write
#include <stdlib.h>         //exit
#include <strings.h>       //bzero
#include <string.h>     //strlen
#include <arpa/inet.h>   // inet_addr
#include<signal.h>

#define BUFFER_SIZE 1024

void SIGINT_HANDLER(int);

void error(char *msg)
{
    perror(msg);
    exit(1);
}

char sendbuffer[1024];      /*buffer to read and write */
char receivebuffer[1024];
int sockfd;

int main(int argc, char *argv[])
{
    int portnumber;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    /*
        test condition for port number entered through command line
    */

    if(argc < 2)
    {
       printf("Enter the port number !! \n");
       printf("Client App is shutting Down\n");
       exit(0);
    }
    /*
        Create a Socket
    */

    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if (sockfd < 0)
    {
        printf("Failed to create socket \n");
	    exit(1);
    }
    else
    {
        printf("Socket is created Successfully\n");
    }
    
    // clear the buffer of structure
    bzero(&serv_addr,sizeof(serv_addr));
    portnumber = atoi(argv[1]);  //port number input through CLI in integer format
    
    //initialize the server address members
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr =  inet_addr("127.0.0.1");  //localhost (loop back IP)
    serv_addr.sin_port =  htons(portnumber); //port number in network byte order
    
    int retval = connect(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr)); 
    if (retval < 0)
    {
        printf("Connection to server is failed \n");
    }
    else
    {
        printf("Connected to the server successfully\n");
    }
  
	  pid_t retf = fork();

	  if(retf < 0)
	  {
		  perror("");
		  exit(3);
	  }

	  if(retf > 0)      //sending to server
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

    if(retf == 0)    //receive from server
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
                printf("Server: %s\n", receivebuffer);
        }
    }
 
    close(sockfd);
    return 0;
} 

void SIGINT_HANDLER(int signum)
{
    strcpy(sendbuffer,"\nClient lost connection\n");
	send(sockfd, sendbuffer, strlen(sendbuffer)+1, 0);
	printf("\nDisconnected Client\n");
	close(sockfd);
	exit(4);
}
