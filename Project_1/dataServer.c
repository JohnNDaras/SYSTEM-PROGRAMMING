// server code for UDP socket programming
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>  
#include <pthread.h>    /* For threads */
#include <signal.h>
#include <poll.h>
#include <errno.h>
#include <sys/signalfd.h>
#include <linux/inotify.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/wait.h>
#include "listforfiles.h" 
#include "queue_server.h" 
#include <ctype.h>
#define IP_PROTOCOL 0
#define PORT_NO 15050
#define NUM_THREADS 5
#define cipherKey 'S'
#define sendrecvflag 0
#define nofile "File Not Found!"
#define perror2(s, e) fprintf(stderr, "%s: %s\n", s, strerror(e))
pthread_mutex_t mtx;         /* Mutex for synchronization */
char *buf;
char *net_buf;
char s[2]="/";
int NET_BUF_SIZE, global_queue_size=0, number_of_files=0, varia=0, var, done = 1, count = 0, sizef, sockfd, nBytes, iter=0;
void *thread_f(void *);      /* Forward declaration */  
FILE* fp;
 
// Declaration of thread condition variable
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
 
// declaring mutex
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


void listFilesRecursively(char *basePath)
{
    int rCode;
    char *path = malloc(500 * sizeof(char)); // Dynamically allocated path buffer
	if (path == NULL) {
		perror("Memory allocation failed");
		exit(EXIT_FAILURE);
	}
    //char path[500];
    struct dirent *dp;
    struct stat statbuf;
    DIR *dir = opendir(basePath);

    // Unable to open directory stream
    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            sizef++;
            number_of_files++;
            // Construct new path from our base path
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
             if (stat(path, &statbuf) == 0 && S_ISREG(statbuf.st_mode)){
            printf("%s \n", path);
            rCode=LIST_InsertTailNodeNames(&Headname, path);
			}
            listFilesRecursively(path);
        }
    }
    closedir(dir);
    free(path);
}

void* foo()
{
   int rCode=0;
   LIST_NAME_T *cur = Headname;
   int nodeCnt=0;
    int i;    
    pthread_mutex_lock(&lock);
   while(cur)
   {
      if(count==global_queue_size)
      {
        //if(varia>0)
        printf("                 Communication thread Pushes another file and waiting again because queue is full\n\n");
		pthread_cond_wait(&cond1, &lock);
		//varia++;
      }
      if(isgraph((cur->payload.name[1])))
      {
         rCode=LIST_InsertTailNode(&listHead, cur->payload.name);
         count++;
      }
       cur=cur->next;
    }
    pthread_mutex_unlock(&lock);
    printf( "\n\tFound %u Files\n", count );	
    // Free the memory allocated for Headname
    FreeListNodes(Headname);
    
    return NULL;
}


// function to clear buffer
void clearBuf(char* b)
{
    int i;
    for (i = 0; i < NET_BUF_SIZE; i++)
        b[i] = '\0';
}
  
// function to encrypt
char Cipher(char ch)
{
    return ch ^ cipherKey;
}
  
// function sending file
int sendFile(FILE* fp, char* buf, int s)
{
    int i, len;
    if (fp == NULL) {
        strcpy(buf, nofile);
        len = strlen(nofile);
        buf[len] = EOF;
        for (i = 0; i <= len; i++)
            buf[i] = Cipher(buf[i]);
        return 1;
    }
  
    char ch, ch2;
    for (i = 0; i < s; i++) {
        ch = fgetc(fp);
        ch2 = Cipher(ch);
        buf[i] = ch2;
        if (ch == EOF)
            return 1;
    }
    return 0;
}


struct sockaddr_in addr_con;
int addrlen = sizeof(addr_con);


void* func1()
{    
     int i,rCode=0;
    // acquire a lock
    pthread_mutex_lock(&lock);

    fp = fopen(net_buf, "r");
     printf("File Name Received: %s\n", net_buf);
     if (fp == NULL){
        printf("\nFile open failed!\n");
        exit(EXIT_FAILURE);
        return NULL;
     }
        
    printf("____________________________________________________________________________\n");

    count--;        
    printf("                 Signaling communication thread to push another file to stack\n");
    done++;
    if(done%2==0)
        listHead = NULL;   
    iter=0;    
    while (1) {           
            iter++;
            if(iter==1)
            {
				printf("iter: %d, %s", iter,net_buf);
                sendto(sockfd, net_buf, NET_BUF_SIZE,
                    sendrecvflag, 
                    (struct sockaddr*)&addr_con, addrlen);
            }  
            else{
            // process
            printf("iter: %d, %s", iter,net_buf);
            if (sendFile(fp, net_buf, NET_BUF_SIZE)) {
                sendto(sockfd, net_buf, NET_BUF_SIZE,
                       sendrecvflag, 
                    (struct sockaddr*)&addr_con, addrlen);
                break;
            }  
            // send
            sendto(sockfd, net_buf, NET_BUF_SIZE,
                   sendrecvflag,
                (struct sockaddr*)&addr_con, addrlen);
            clearBuf(net_buf);
        }    
     }
    pthread_cond_signal(&cond1);
    // release lock
    pthread_mutex_unlock(&lock); 
    return NULL;        
}



// driver code
int main(int argc, char *argv[])
{    
    int port=15050;
    int thread_pool_size=4;
    int queue_size=4;
    int block_size=520;
    int rCode,y;
    int rc,c=0;
    int i,err;
    buf = malloc(50 * sizeof(char)); // Dynamically allocated buffer
    if (buf == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    net_buf = malloc(1000 * sizeof(char)); // Dynamically allocated net buffer
    if (net_buf == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    printf("Program name %s, second is %s\n", argv[0],argv[1]);
    if( argc == 2 ) {
        printf("The argument supplied is %s\n", argv[1]);
    }
    else if( argc > 2 ) {
        //printf("Too many arguments supplied.\n"); 
        if(strcmp(argv[1],"-p")==0){
            port = atoi(argv[2]);
            if(strcmp(argv[3],"-s")==0){
                thread_pool_size = atoi(argv[4]);
                numberof(thread_pool_size);             
                if(strcmp(argv[5],"-q")==0){              
                    queue_size = atoi(argv[6]);                   
                    if(strcmp(argv[7],"-b")==0){              
                        block_size = atoi(argv[8]);                          
                    }
                    else{
                        printf("Sorry,you need to follow this order\n./dataServer -p <port> -s <thread_pool_size> -q <queue_size> -b <block_size>\n");
                        printf("You probably meant '-b' instead of %s\n\n",argv[5]);
                        return 1;
                    }                    
                }
                else{
                    printf("Sorry,you need to follow this order\n./dataServer -p <port> -s <thread_pool_size> -q <queue_size> -b <block_size>\n");
                    printf("You probably meant '-q' instead of %s\n\n",argv[5]);
                    return 1;
                }
            }
            else{
               printf("Sorry,you need to follow this order\n../dataServer -p <port> -s <thread_pool_size> -q <queue_size> -b <block_size>\n"); 
               printf("You probably meant '-s' instead of %s\n\n",argv[3]);
               return 1;
            }
      }
      else{         
          printf("Sorry,you need to follow this order\n../dataServer -p <port> -s <thread_pool_size> -q <queue_size> -b <block_size>\n");
          printf("You probably meant '-p' instead of %s\n\n",argv[1]);
          return 1;
      }
   }     
    printf("\nServer’s parameters are:");
    printf("\nport: %d\nthread_pool_size: %d\nqueue_size: %d\nblock_size: %d\n", port,thread_pool_size,queue_size,block_size);
    NET_BUF_SIZE = block_size;
    global_queue_size=queue_size;
    addr_con.sin_family = AF_INET;
    addr_con.sin_port = htons(port);
    addr_con.sin_addr.s_addr = INADDR_ANY;
    char path[500];
    // socket()
    sockfd = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL);
    if (sockfd < 0)
        printf("\nfile descriptor not received!!\n");
    else
        printf("\nfile descriptor %d received\n", sockfd);
    // bind()
    if (bind(sockfd, (struct sockaddr*)&addr_con, sizeof(addr_con)) == 0)
        printf("\nSuccessfully binded!\n");
    else
        printf("\nBinding Failed!\n");
    printf("\nWaiting for file name...\n");
  
    // receive file name
    clearBuf(net_buf);
    nBytes = recvfrom(sockfd, net_buf,NET_BUF_SIZE, sendrecvflag,(struct sockaddr*)&addr_con, &addrlen);                               
    strcpy(buf,net_buf);
    listFilesRecursively(buf);   //find files and subdirectories of our directory path
    pthread_t tid1, tid2;
    pthread_mutex_init(&lock, NULL);
    
    // Create thread 1
    pthread_create(&tid1, NULL, foo, NULL);
    // sleep for 1 sec so that thread 1
    // would get a chance to run first
    sleep(1);
    //pthread_join(tid1, NULL);
    printf("\n\n\nCommunication thread with id=%ld added these elements to queue so far\n",tid1);
    rCode=PrintListPayloads(listHead);

    pthread_t threads[NUM_THREADS];
    LIST_NODE_T *cur = listHead;
    i=0;
    /* deref head_ref to get the real head */
    LIST_NODE_T* current = listHead;
    LIST_NODE_T* next;
    printf("\n\n\n");
   
    while (current != NULL)
    {
//      printf("\n\n\n\n%s\n",current->payload.name);
        printf("\n\n\n\n\n");
        c++;
        for( i = 0; i <= thread_pool_size; i++ ) {
       
          strcpy(net_buf, current->payload.name);
          rc = pthread_create(&threads[i], NULL, func1, NULL);
          printf("[%ld]",threads_id[i]);
          if (rc) {
             printf("Error:unable to create thread, %d\n", rc);
             exit(-1);
          }          
          if (err = pthread_mutex_unlock(&lock)) { /* Unlock mutex */
            perror2("pthread_mutex_unlock", err); exit(1); 
          }
          if (err = pthread_join(threads[i], NULL)) { /* Wait for thread */
            perror2("pthread_join", err); exit(1); } /* termination */
            c++;
          
          if(current->next!=NULL)
          {
              if(c<=20 && i!=thread_pool_size)
              {
                if(isgraph((current->next->payload.name[1])))  
                    next = current->next;
                free(current);  //pop element from queue so communication thread can push another one in it 
                current = next;
              }
              else
                break;                
          }
          else
             current->next=listHead;
                         
          if(c==number_of_files-1)
            return 0;   
       }   
       
        if(current->next!=NULL)
        {
            if(c<=20  && current->next!=NULL)
            {
                if(isgraph((current->next->payload.name[1]))) 
                    next = current->next;
                free(current);  //pop element from queue so communication thread can push another one in it
                current = next;
            }
            else{            
                break;
            }
        }
        else
            current->next=listHead;
               
     if(c==number_of_files-1)
         return 0;   
        
        
   }
   
   /* deref head_ref to affect the real head back
      in the caller. */
   listHead = NULL;   
   
    if (fp != NULL)
            fclose(fp);

    free(buf);
    free(net_buf);
    return 0;
}
