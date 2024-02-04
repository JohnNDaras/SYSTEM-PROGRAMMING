// client code for UDP socket programming
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>  
#include "stringhandleclient.h"    
#define IP_PROTOCOL 0
#define IP_ADDRESS "127.0.0.1" // localhost
#define PORT_NO 15050
#define NET_BUF_SIZE 320
#define cipherKey 'S'
#define sendrecvflag 0


struct sockaddr_in addr_con;
int addrlen = sizeof(addr_con);

void func(int sockfd,char *net_buf)
{char string[] = "good bye";
    FILE * fPtr;
    int nBytes,iter=0,epan=0;
    long int stringlen;
    char net_buf1[NET_BUF_SIZE+NET_BUF_SIZE];
    char net_buf2[NET_BUF_SIZE+NET_BUF_SIZE];  
    while (1) {
        epan++;
        if(epan==1)
        {   
            printf("\nPlease enter file name to receive:\n");
            //scanf("%s", net_buf);
            stringlen = strlen(net_buf);
            stringhandle(net_buf);
            sendto(sockfd, net_buf, NET_BUF_SIZE,
                   sendrecvflag, (struct sockaddr*)&addr_con,
                   addrlen);
        }
        iter=0;
        while (1) {
            // receive
            iter++;
            if(iter==1)
            {
                 nBytes = recvfrom(sockfd, net_buf, NET_BUF_SIZE, sendrecvflag, (struct sockaddr*)&addr_con, &addrlen);               
                 revstr(net_buf);   
                 net_buf[strlen(net_buf)-stringlen-1] = '\0';
                 revstr(net_buf);
                 sprintf(net_buf1, "%s%s%s", final,slash,net_buf);
                 printf("%s\n",net_buf1); 
                 strcpy(net_buf2,net_buf1);
                 createdirectory(net_buf1);
                fPtr = fopen(net_buf2, "w+");
            }  
            else{         
                clearBuf(net_buf);
                nBytes = recvfrom(sockfd, net_buf, NET_BUF_SIZE,
                                  sendrecvflag, (struct sockaddr*)&addr_con,
                                  &addrlen);
                // process
                //printf("netbuf:  %s\n\n", net_buf2);
                if (recvFile(net_buf, NET_BUF_SIZE,fPtr)) {
                    break;
                }
            }
        }
    }
}


  
// driver code
int main(int argc, char *argv[])
{   
    int server_port;
    char *server_ip = malloc(100 * sizeof(char)); // Dynamically allocated path buffer
    if (server_ip== NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    char *path = malloc(500 * sizeof(char)); // Dynamically allocated path buffer
    if (path == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    printf("Program name %s, second is %s\n", argv[0],argv[1]);
 
    if( argc == 2 ) {
        printf("The argument supplied is %s\n", argv[1]);
    }
   
    else if( argc > 2 ) {
        //printf("Too many arguments supplied.\n");
        if(strcmp(argv[1],"-i")==0){
            strcpy(server_ip ,argv[2]);
            if(strcmp(argv[3],"-p")==0){
                server_port = atoi(argv[4]);
                if(strcmp(argv[5],"-d")==0){
                    strcpy(path,argv[6]);
                }
                else{
                    printf("Sorry,you need to follow this order\n./remoteClient -i <server_ip> -p <server_port> -d <directory>\n");
                    printf("You probably meant '-d' instead of %s\n\n",argv[5]);
                    return 1;
                }
            }
            else{
               printf("Sorry,you need to follow this order\n./remoteClient -i <server_ip> -p <server_port> -d <directory>\n"); 
               printf("You probably meant '-p' instead of %s\n\n",argv[3]);
               return 1;
            }
      }
      else{     
          printf("Sorry,you need to follow this order\n./remoteClient -i <server_ip> -p <server_port> -d <directory>\n");
          printf("You probably meant '-i' instead of %s\n\n",argv[1]);
          return 1;
      }
   }  
     printf("\nClient’s parameters are:");
     printf("\nserverIP: %s\nport: %d\ndirectory: %s\n",server_ip, server_port ,path);
    int sockfd,nBytes;
    addr_con.sin_family = AF_INET;
    addr_con.sin_port = htons(server_port);
    addr_con.sin_addr.s_addr = inet_addr(server_ip);
    char net_buf[NET_BUF_SIZE];
    FILE* fp;
    // socket()
    sockfd = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL);
    if (sockfd < 0)
        printf("\nfile descriptor not received!!\n");
    else
        printf("\nfile descriptor %d received\n", sockfd);
    func(sockfd, path);
    free(path);
    free(server_ip);
    return 0;
}
