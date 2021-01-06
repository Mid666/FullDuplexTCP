/*
	Midhun Nair
	Updated Date: 6/1/2021 

*/

#include<sys/types.h>     //socket, bind, listen, accept API s
#include<sys/socket.h>
#include<unistd.h>        //read, write
#include<netinet/in.h>
#include<netinet/ip.h>
#include<string.h>
#include<stdlib.h>
#include<signal.h>

void SIGINT_HANDLER(int);

char sendbuffer[1024];
char receivebuffer[1024];

int sockfd;    // socket descriptor to listen incoming connection request.
int connfd;    // socket descriptor to establish connection with client, for accept system call to be used for sending responses. 

int main(int argc, char *argv[])
{
  int portnumber, client_addlen;
  int readbytes;
  
  if(argc < 2)
  {
      printf("Error: Pass the Server port no. as first argument above 1023\n");
      exit(1);
  }
  
  /*   struct sockaddr_in
   *   {
   *      sa_family sin_family; // family(ipv4/ipv6) [AF_INET:ipv4]     
   *      in_port_t sin_port;   // port in network byte order
   *      struct in_addr sin_addr; //only ip address
   *   };
   *    
   *    struct in_addr 
	 *    {
   *       uint32_t       s_addr;    // address in network byte order
   *    };
   */
   
   struct sockaddr_in server_addr;
   struct sockaddr_in client_addr;
   
   bzero((char *)&server_addr, sizeof(server_addr));
   bzero((char *)&client_addr, sizeof(client_addr));
   
   portnumber = atoi(argv[1]);   
   
   sockfd = socket(AF_INET, SOCK_STREAM, 0);  // SOCK_STREAM: TCP
   
   if(sockd < 0)
   {
      perror("Err: Failed to create socket\n");
      exit(2);
   }
   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr = INADDR_ANY;  //any address
   server_addr.sin_port = htons(portnumber);   //add and port are stored in network byte order, hence htons
   
    /* When a socket is created it assumes the IP address of the network node that created it. 
	  If a socket has an IP address but not a port number it is said to be ‘unbound’. 
	  An unbound socket cannot receive data because it does not have a complete address */
    
    int ret = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if(ret < 0)
    {
      perror("Err: Failed to bind the socket. UNBOUND SOCKET.\n");
      exit(3);
    }
    
    int pending_requests = 5; //MAX 5 pending requests in queue
    int retval = listen(sockd, pending_requests); //listen to max 5 requests through sockfd.
    if(retval < 0)
    {
      perror("Err:Failed to accept request\n");
      exit(4);
    }
    
    client_addlen = sizeof(client_addr);
    
    /* To establish connection with client: To accept client */
    connfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addlen);
    if(connfd < 0)
    {
      perror("Error in fetching connection request\n");	
      exit(5);
    }
    
    /*************SENDER-RECEIVER*********************/
    
    pid_t retf = fork();
    if(retf < 0)
    {
      perror("");
      exit(6);
    } 
    
    /****SENDING TO CLIENT****/
    
    if(retf > 0)
    {
      while(1)
      {
        signal(SIGINT, SIGINT_HANDLER);
        bzero(&sendbuffer, sizeof(sendbuffer));
        fgets(sendbufffer, sizeof(sendbuffer), stdin);
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
          bzero(&receivebuffer, sizeof(receivebuffer));
          size_t rret = recv(connf, receivebuffer, sizeof(receivebuffer), 0);
          if(!strcmp(receivebuffer, "Disconnect\n")
          {
            printf("Diconnecting..\nDisconnected\n");
            exit(0);
          }
          printf("Client: %s\n", receivebuffer);
        }
    }
    
  close(connfd);
  close(sockfd);
  return 0;  
}    

void SIGINT_HANDLER(int signum)
{
  strcpy(sendbuffer,"\nServer Disconnected\n");
  send(connfd, sendbuffer, strlen(sendbuffer)+1, 0);
  printf("\nDisconnected Server\n");
  close(connfd);
  close(sockfd);
  exit(2);
}
    
   


