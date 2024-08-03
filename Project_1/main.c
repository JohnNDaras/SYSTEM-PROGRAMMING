#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>
#include <errno.h>
#include <sys/signalfd.h>
//#include <linux/inotify.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include "list_for_files.h"
#include "list_for_ip.h"
#include "inotifywait.h"
#include "string_handling.h"
#include <sys/wait.h>
#include <fcntl.h>

#define DATA_SIZE 1000
#define BUF_SIZE 256
#define PATH_SIZE 200

int number_of_files=0;
	
int main (int argc,const char **argv)
{		
    int signal_fd, inotify_fd, rCode, f1,f2, fifo_write,fifo_read, nstrcount=0,nnstrcount=0, u=0, count = 0, simaia1=0;    
    char str[BUF_SIZE], ns[PATH_SIZE] , nstr[PATH_SIZE], final[300], path[PATH_SIZE],  buffer[INOTIFY_BUFFER_SIZE], newString[DATA_SIZE][DATA_SIZE], nnstr[PATH_SIZE], output[18]= "outputsdi1800040", slash[2]="/",  strout[8]=".out";   
    struct dirent *res;
    struct stat sb;	
    struct pollfd fds[FD_POLL_MAX];
  
  	// Variables we use for making output files
	FILE * fp;
	char * line = NULL;
	size_t len = 0, read_file;
	int line_num = 1, find_result = 0, i=0,j,ctr,z,w, arithmos;

  	//Variables we use to extract location from urls 
	int succ_parsing = 0; // Whether the parsing has been
	char ip[100]; // IP field of the HTTP uri
	int port;  // Port field of the HTTP uri if found
	char page[PATH_SIZE]; // Page field of the uri if found
	char* host; // Will be the ip and page joined in one char*
	char tmp_inside_lower[DATA_SIZE]; // Tmp variable if the uri contains < and >
	char* tmp_source; // Points to the source to parse
	char conc[DATA_SIZE];
	/* Variable to store user content */
	char data[DATA_SIZE];

	/* File pointer to hold reference to our file */
	FILE * fPtr;
  /*   Create named pipe (From Listener to Manager) */ 
    f1 = mkfifo("pipeIIIII",0666);
    
   /*Create a directory by handling path string (for example if we have a path "/home/syspro/test", we 'll cretae the directory "/home/syspro/outputsdi1800040" */
    strcpy(&ns[0],argv[1]);   
    revstr(ns);
    nstrcount = strcspn(ns, "/");		
    strncpy(nstr, ns, nstrcount);
    nstr[nstrcount] = '\0'; 
    revstr(nstr);
    revstr(ns);
    ns[strlen(ns)-nstrcount-1] = '\0';

    sprintf(final, "%s%s%s", ns,slash,output);
    printf("Teliko:\n%s\n", final);
    int result = mkdir(final, 0777);
    strcpy(&path[0],argv[1]);
										
	/*	Count our files alraedy exist in our directory so we can make equal number workers */																		
    if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode)){
    DIR *folder = opendir ( path );

    if (access ( path, F_OK ) != -1 ){
        if ( folder ){
            while ( ( res = readdir ( folder ) ) ){
                if ( strcmp( res->d_name, "." ) && strcmp( res->d_name, ".." ) ){
                    printf("%u) - %s\n", count + 1, res->d_name);
                    count++;
                }
            }
            closedir ( folder );
        }else{
            perror ( "Could not open the directory" );
            exit( EXIT_FAILURE);
        }
    }
    }else{
        printf("The %s it cannot be opened or is not a directory\n", path);
        exit( EXIT_FAILURE);
    }

    printf( "\n\tFound %u Files\n", count );										
  
    int fd[2];
    if (pipe(fd) == -1) {
        printf("An error ocurred with opening the pipe\n");
        return 1;
    }
  
  
  //__________________________________      LISTENER  ______________________________//
  
	pid_t listener; //parent
	listener = fork();
	
	if (listener == -1) 
	{ 
		perror("failed to fork");
		exit (23);
	}

	if (listener > 0) 
	{ 	
		close(fd[0]);
	    if (write(fd[1], &count, sizeof(int)) == -1) 
	    {
	            printf("An error ocurred with writing to the pipe\n");
	            return 3;
	     }
	     close(fd[1]);			
			
		/* Input arguments... */
		if (argc < 2)
	    {
			fprintf (stderr, "Usage: %s directory1 [directory2 ...]\n", argv[0]);
			exit (EXIT_FAILURE);
	    }
	
	  /* Initialize signals FD */
		if ((signal_fd = __initialize_signals ()) < 0)
	    {
			fprintf (stderr, "Couldn't initialize signals\n");
			exit (EXIT_FAILURE);
	    }
	
	  /* Initialize inotify FD and the watch descriptors */
		if ((inotify_fd = __initialize_inotify (argc, argv)) < 0)
	    {
			fprintf (stderr, "Couldn't initialize inotify\n");
			exit (EXIT_FAILURE);
	    }
	
		/* Setup polling */
		fds[FD_POLL_SIGNAL].fd = signal_fd;
		fds[FD_POLL_SIGNAL].events = POLLIN;
		fds[FD_POLL_INOTIFY].fd = inotify_fd;
		fds[FD_POLL_INOTIFY].events = POLLIN;
	
																			//	INOTIFY PROCEDURE //
	
		/* Now loop */
		for (;;)
	    {
			/* Block until there is something to be read */
			if (poll (fds, FD_POLL_MAX, -1) < 0)
	        {
				fprintf (stderr,
	                   "Couldn't poll(): '%s'\n",
				strerror (errno));
				exit (EXIT_FAILURE);
			}
	
	      /* Signal received? */
			if (fds[FD_POLL_SIGNAL].revents & POLLIN)
	        {
				struct signalfd_siginfo fdsi;
	
				if (read (fds[FD_POLL_SIGNAL].fd, &fdsi, sizeof (fdsi)) != sizeof (fdsi))
	            {
	              		fprintf (stderr,"Couldn't read signal, wrong size read\n");
	              		exit (EXIT_FAILURE);
	            }
	
				/* Break loop if we got the expected signal */
				if (fdsi.ssi_signo == SIGINT || fdsi.ssi_signo == SIGTERM)
	            {
					break;
	            }	
				fprintf (stderr,"Received unexpected signal\n");
			}
	
			/* Inotify event received? */
			if (fds[FD_POLL_INOTIFY].revents & POLLIN)
	        {
				char buffer[INOTIFY_BUFFER_SIZE];
				size_t length;	
				/* Read from the FD. It will read all events available up to
				* the given buffer size. */
				if ((length = read (fds[FD_POLL_INOTIFY].fd, buffer, INOTIFY_BUFFER_SIZE)) > 0)
	            {
					struct inotify_event *event;	
					event = (struct inotify_event *)buffer;
					while (IN_EVENT_OK (event, length))
	                {
						__event_process (event);
						event = IN_EVENT_NEXT (event, length);
						fifo_write= open("pipeIIIII",O_WRONLY);
						if(fifo_write<0)
							printf("\nError opening pipe");
						else
						{
							write(fifo_write,pipebuf,255*sizeof(char));							//HERE WE WRITE OUR PATH TO FIFO (from listener to manager) (pipebuf = "full path")
							close(fifo_write);						
						}		  				  
					}
				}
	        }
	    }
	  
	  	/* Clean exit */
		__shutdown_inotify (inotify_fd);
		__shutdown_signals (signal_fd);
	
		printf ("Exiting program...\n");
	}

				/*/________________________________________END OF LISTENER  ________________________________________*/


				/*/___________________________________________    MANAGER ____________________________________________/*/
																								
	if(listener == 0)
	{
		char str1[BUF_SIZE];       //this is string manager receives from listener through FIFO
		int fifo_write1;	
			
		char str2[BUF_SIZE];     	//this is string workers receive from manager through FIFO
		int fifo_read2;	
		
		//Pipe we use to write from manger to worker
		f2 = mkfifo("pipeJJJJJ",0666);												
																				
	    close(fd[1]);
	    if (read(fd[0], &count, sizeof(int)) == -1) 
	    {
	       printf("An error ocurred with reading from the pipe\n");
	       return 4;
	     }
	    close(fd[0]);																
																				

	    pid_t children[count];
	    pid_t pid;
   
		/* At first let's make as many workers as our already existing files  */
	    for(size_t i = 0; i < count; i++) 
	    {
			pid = fork();
			if(pid == -1) {
			    perror("parent: error forking");
			    break;
			}

			if(pid == 0) 
			{
		         raise(SIGSTOP); // child stops itself              
		         while(strcmp(str1,"end")!=0)
			    {
				fifo_read2=open("pipeJJJJJ",O_RDONLY);   //  here is FIFO, worker uses to read from manager 
				if(fifo_read2<0)
				    printf("\nError opening read pipe");
				else
				{
				    read(fifo_read2,str2,255*sizeof(char));  // worker reads from manager
				    close(fifo_read2);	
				    printf("\n\nWorker received string:\n%s",str2);

					/* Here we take our path from FIFO (str2), we extract our file and we store it to nnstr (FOR EXAMPLE if we have "/home/syspro/test/file1.txt" we extract file1.txt)*/	
				    revstr(str2);    								//reverse string (function in string_handling.h
				    nstrcount = strcspn(str2, "/");					
				    strncpy(nnstr, str2, nstrcount);
				    nnstr[nstrcount] = '\0'; 
			        revstr(nnstr);
				    revstr(str2);
		
					
		/* ////////////////////////////////// Create our path for directory which includes our output files ///////////////////////////////////////////////
		* ( (FOR EXAMPLE if we have str2="/home/syspro/test/file1.txt" then our path is "/home/syspro/outputsdi1800040") */	
		
				    strcpy(ns,str2);
				    for(u=0; u<2;u++)
				    {
						revstr(ns);
						nstrcount = strcspn(ns, "/");					
						strncpy(nstr, ns, nstrcount);
						nstr[nstrcount] = '\0'; 
						revstr(nstr);
						revstr(ns);
						ns[strlen(ns)-nstrcount-1] = '\0';
				    }
				    sprintf(final, "%s%s%s%s", ns,slash,output,slash);
				    printf("\n\n");
	
	
			/* ///////////////////////////////////////////////  Create our final path for our output files  /////////////////////////////////////////////////////////////////////////////
			* ( (FOR EXAMPLE if we have str2="/home/syspro/test/file1.txt" then our path is "/home/syspro/outputsdi1800040/file1.txt.out") */	
			
				     sprintf(conc, "%s%s%s",final, nnstr,strout);	
				     printf("\nHere is output file location:\n%s\n\n\n",conc);
				     fPtr = fopen(conc, "w");
			/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
					
					/* fopen() return NULL if last operation was unsuccessful */
				      if(fPtr == NULL)
				      {
						  /* File not created hence exit */
						  perror("Unable to create file.\n");
						  exit(EXIT_FAILURE);
				      }
	
				      number_of_files++;
				      fp = fopen(str2, "r");
				      if (fp == NULL)
					  exit(EXIT_FAILURE);					
	
	
	/* //////////////////////////////////  Here we find all URLs which start with http  //////////////////////////////////////////////////*/
								
				      while ((read_file = getline(&line, &len, fp)) != -1) 
				      {
					
						/*___________ We first find all lines which contain http___________*/
					
					    if((strstr(line, "http") != NULL)) {
					    printf("\n\nRetrieved line of length %zu:\n", read_file);
					    printf("A match found on line: %d\n", line_num);			
					    printf("\n%s\n", line);
						
						
					    arithmos = (int)read_file;
					    char str1[arithmos];
					    strcpy(str1,line);	
						
					/* _____________Then we split them to tokens_______________-*/
					    j=0; ctr=0;
					    for(z=0;z<=arithmos;z++)
					    {
							 // if space or NULL found, assign NULL into newString[ctr]
							 if(str1[z]==' '||str1[z]=='\0')
							 {
								   newString[ctr][j]='\0';
								   ctr++;  //for next word
								   j=0;    //for next word, init index to 0
							 }
							  else
							 {
								newString[ctr][j]=str1[z];
								j++;
							 }
					     }
						
						/*_______________Then we extract location from urls if the token is url type_______________*/
						
					     for(z=0; z< ctr;z++)
					     {				  
						   memset(ip, 0, 100);
						   memset(page, 0, 200);
						   memset(tmp_inside_lower, 0, 1000);
						   port = 80;
						   succ_parsing = 0;
	
						   // Set the proper tmp_source char*
						   int n1 = sscanf(newString[z], "<%999[^>]>", tmp_inside_lower);
						   if (n1 > 0) { tmp_source = tmp_inside_lower;}
						   else { tmp_source = newString[z];}
	
						   // Parsing the tmp_source char*
						   if (sscanf(tmp_source, "http://%99[^:]:%i/%199[^\n]", ip, &port, page) == 3) { succ_parsing = 1;}
						   else if (sscanf(tmp_source, "http://%99[^/]/%199[^\n]", ip, page) == 2) { succ_parsing = 1;}
						   else if (sscanf(tmp_source, "http://%99[^:]:%i[^\n]", ip, &port) == 2) { succ_parsing = 1;}
						   else if (sscanf(tmp_source, "http://%99[^\n]", ip) == 1) { succ_parsing = 1;}
						   if (sscanf(tmp_source, "http://%*3[w].%99[^:]:%i/%199[^\n]", ip, &port, page) == 3) {succ_parsing = 1;}
						   else if (sscanf(tmp_source, "http://%*3[w].%99[^/]/%199[^\n]", ip, page) == 2) { succ_parsing = 1;}
						   else if (sscanf(tmp_source, "http://%*3[w].%99[^:]:%i[^\n]", ip, &port) == 2) { succ_parsing = 1;}
						   else if (sscanf(tmp_source, "http://%*3[w].%99[^\n]", ip) == 1) {succ_parsing = 1;}


	
						   // Properly attaching the ip+page to a host
						   if (succ_parsing) {
							char *host;
							if (page != NULL && strlen(page) > 0) 
							{
								size_t len = snprintf(NULL, 0, "%s/%s", ip, page);
								host = (char *)malloc(len + 1);
								if (host == NULL) 
								{
									perror("malloc");
									exit(EXIT_FAILURE);
								}
								snprintf(host, len + 1, "%s/%s", ip, page);
							} else {
								size_t len = snprintf(NULL, 0, "%s", ip);
								host = (char *)malloc(len + 1);
								if (host == NULL) {
									perror("malloc");
									exit(EXIT_FAILURE);
								}
								snprintf(host, len + 1, "%s", ip);
							}

							// Some printing
							printf("--------------------- \n");
							printf("test = \"%s\"\n", newString[z]);
							removeChar(ip, '/');
							printf("*ip = \"%s\"\n", ip);

							/*__We append them to list of structs (struct contains location_name and number of appearances (you can look list_for_ip.h)).*/
							LISTsearchByNameIp(listHeadIp, ip);
							if (simaiaip == 0) // if location doesn't already exist in list
								rCode = LIST_InsertTailNodeIp(&listHeadIp, ip, 1); // then append it to list with number of appearance = 1
							else
								rCode = LISTIncreaseCounterIp(listHeadIp, ip); // else find this location in list and increase number of appearances by 1
							free(host);
						 }
						}			
						find_result++;
					 }
					   line_num++;
				   }	  
				     rCode=PrintListPayloadsIp(listHeadIp, fPtr);          // then print our list to our file in our output directory 
		   
		                      /* Close file to save file data */
		        	  fclose(fPtr);
		
			          if(i!=0)
				         kill(getpid(),SIGINT);											// kill worker process
					
				}	
				__shutdown_inotify (inotify_fd);
				__shutdown_signals (signal_fd);
			    }	
								
		    } else 
		     {
       			 children[i] = pid;
     		}
   		}	
	    	
					/*/ ________________HERE IS OUR MAIN MANAGER PROCESS______________/*/												
	
	        while(strcmp(str,"end")!=0)
	        {
	             fifo_read=open("pipeIIIII",O_RDONLY);           // HERE WE RECEIVE OUR STRING FROM LISTENER AND LATER WE SEND IT TO WORKER  //
	             if(fifo_read<0)
	                  printf("\nError opening read pipe");
	             else
	            {
					read(fifo_read,str,255*sizeof(char));		
					LISTsearchByName(listHead,str);
							
					if(simaia==0)																	// if file doesn't already exists to heap 
					{
						i++;
						simaia1 = 1;
						rCode=LIST_InsertTailNode(&listHead, str);				// then push into heap 			 			 
						printf("\n________________________________________\n");
						printf("\nThese are the files inotify procedure noticed during running\n\n");
						rCode=PrintListPayloads(listHead);
						
						if(i>=count)
						{
							if ( (pid = fork()) <= 0)
							break;
							wait(NULL);	
						}
						else
						{
							fprintf(stdout, "\nManager: signaling child (%d) to continue...\n", children[i]);
						}
						kill(children[i], SIGCONT);
			
		/*/////////////////////////////////////////    Write from Mangers to Worker (The path that manager received from listener    //////////////////////////////////////*/
						fifo_write1= open("pipeJJJJJ",O_WRONLY);
						if(fifo_write1<0)
							printf("\nError opening pipe");
						else
						{
							write(fifo_write1,str,255*sizeof(char));
							close(fifo_write1);						
						}												
							close(fifo_read);	
					}	
					close(fifo_read);
	            }
	        }  	  
	  
	        __shutdown_inotify (inotify_fd);
		__shutdown_signals (signal_fd);
        } 
}

