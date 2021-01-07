/*
    MIDHUN NAIR
   
    Date:   07/01/2021

    TCP-SERVER IMPLEMENTATION

*/

#include<stdio.h>
#include<sys/types.h>     
#include<sys/socket.h>
#include<unistd.h>      
#include<netinet/in.h>
#include<netinet/ip.h>
#include<string.h>
#include<stdlib.h>
#include<signal.h>

void SIGINT_HANDLER(int);

char sendbuffer[1024];
char receivebuffer[1024];

int sockfd;   
int connfd;   

int main(int argc, char *argv[])
{
    int portnumber, client_addlen;
    int readbytes;
  
    if(argc < 2)
    {
        printf("Error: Pass the Server port no. as first argument above 1023\n");
        exit(EXIT_FAILURE);
    }
   
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
   
    bzero((char *)&server_addr, sizeof(server_addr));
    bzero((char *)&client_addr, sizeof(client_addr));
   
    portnumber = atoi(argv[1]);   /* converting string to int */
   
    sockfd = socket(AF_INET, SOCK_STREAM, 0);  
   
    if(sockfd < 0)
    {
        perror("Err: Failed to create socket\n");
        exit(EXIT_FAILURE);
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  
    server_addr.sin_port = htons(portnumber);  
    
    int ret = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if(ret < 0)
    {
        perror("Err: Failed to bind the socket. UNBOUND SOCKET.\n");
        exit(EXIT_FAILURE);
    }
    else
        printf("Binding successful\n");    

    int pending_requests = 5;
    int retval = listen(sockfd, pending_requests);
    if(retval < 0)
    {
        perror("Err:Failed to accept request\n");
        exit(EXIT_FAILURE);
    }
    else 
        printf("Listening....\n");

    client_addlen = sizeof(client_addr);

    connfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addlen);
    if(connfd < 0)
    {
        perror("Error in fetching connection request\n");	
        exit(EXIT_FAILURE);
    }
    else
        printf("Connected to Client\n\n");

    /*************SENDER-RECEIVER*********************/
    
    pid_t retf = fork();
    if(retf < 0)
    {
        perror("");
        exit(EXIT_FAILURE);
    } 
    
    /****SENDING TO CLIENT****/
    
    if(retf > 0)
    {
        while(1)
        {
            signal(SIGINT, SIGINT_HANDLER);
            bzero(&sendbuffer, sizeof(sendbuffer));
            fgets(sendbuffer, sizeof(sendbuffer), stdin);
            size_t sret = send(connfd, sendbuffer, strlen(sendbuffer)+1, 0);
            if(sret < 0)
            {
                printf("\nsending failed..Try Again\n");
            }
        }
    }
   
    /****RECEIVING FROM CLIENT****/
    
    if(retf == 0)
    {
        while(1)
        {
            signal(SIGINT, SIGINT_HANDLER);
            bzero(&receivebuffer, sizeof(receivebuffer));
            size_t rret = recv(connfd, receivebuffer, sizeof(receivebuffer), 0);
            if(!strcmp(receivebuffer, "Disconnect\n"))
            {
                printf("Diconnecting..\nDisconnected\n");
                exit(0);
            }
            printf("Msg from Client: %s\n", receivebuffer);
        }
    } 
    close(connfd);
    close(sockfd);
    return 0;  
}    

/* SIGNAL HANDLER IF USER PRESS CTRL+C */

void SIGINT_HANDLER(int signum)
{
    strcpy(sendbuffer,"\nServer Disconnected\n");
    send(connfd, sendbuffer, strlen(sendbuffer)+1, 0);
    printf("\nDisconnected Server\n");
    close(connfd);
    close(sockfd);
    exit(EXIT_FAILURE);
}
    
