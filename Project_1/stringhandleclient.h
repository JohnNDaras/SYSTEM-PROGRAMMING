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
  
#define IP_PROTOCOL 0
#define IP_ADDRESS "127.0.0.1" // localhost
#define PORT_NO 15050
#define NET_BUF_SIZE 320
#define cipherKey 'S'
#define sendrecvflag 0
char final[300];
char slash[2]="/";
  
// function to clear buffer
void clearBuf(char* b)
{
    int i;
    for (i = 0; i < NET_BUF_SIZE; i++)
        b[i] = '\0';
}
  
// function for decryption
char Cipher(char ch)
{
    return ch ^ cipherKey;
}
  
// function to receive file

int recvFile(char* buf, int s, FILE *fptr)
{
	char* finalString = (char*)malloc((s + 1) * sizeof(char));  // Allocate memory for the final string
    int i;
    char ch;
    for (i = 0; i < s; i++) {
        ch = buf[i];
        ch = Cipher(ch);
        if (ch == EOF)
        {
		    fprintf(fptr, "%s", finalString);
            return 1;
         }   
        else
			finalString[i] = ch;
            fprintf(fptr, "%c", ch);
            printf("%c", ch);
    }
    fprintf(fptr, "%s", finalString);
    return 0;
}

/*
int recvFile(char* buf, int s, FILE *fptr) {
    char* finalString = (char*)malloc((s + 1) * sizeof(char));  // Allocate memory for the final string
    if (finalString == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return 0;
    }

    int i;
    for (i = 0; i < s; i++) {
        char ch = buf[i];
        ch = Cipher(ch);
        if (ch == EOF)
            break;

        finalString[i] = ch;
        printf("%c", ch);
    }
    finalString[i] = '\0';  // Null-terminate the string

    fprintf(fptr, "%s", finalString);  // Write the entire string to the file
    fflush(fptr);  // Flush the file buffer to ensure immediate write

    free(finalString);  // Free the allocated memory
    return 0;
}
*/
void revstr(char *str1)  
{  
    // declare variable  
    int i, len, temp;  
    len = strlen(str1); // use strlen() to get the length of str string  
      
    // use for loop to iterate the string   
    for (i = 0; i < len/2; i++)  
    {  
        // temp variable use to temporary hold the string  
        temp = str1[i];  
        str1[i] = str1[len - i - 1];  
        str1[len - i - 1] = temp;  
    }  
}   




void removeChar(char * str, char charToRemmove){
    int i, j;
    int len = strlen(str);
    for(i=0; i<len; i++)
    {
        if(str[i] == charToRemmove)
        {
            for(j=i; j<len; j++)
            {
                str[j] = str[j+1];
            }
            len--;
            i--;
        }
    }
    
}


char *stringhandle(char *s){
    
    char ns[200];
    strcpy(&ns[0],s);
    char ns1[200];
    char nstr[200];
    //char final[300] ;
    char output[30]= "outputsdi1800040";       
    //char slash[2]="/";
    int nstrcount=0,u=0;

    

	revstr(ns);
	nstrcount = strcspn(ns, "/");		
	strncpy(nstr, ns, nstrcount);
	nstr[nstrcount] = '\0'; 
	revstr(nstr);
	revstr(ns);
	ns[strlen(ns)-nstrcount-1] = '\0';

    sprintf(final, "%s%s%s", ns,slash,output);
    return final;
    //printf("%ld\n",strlen(ns));
    //printf("Teliko:\n%s\n", final);
    
}




void createdirectory(const char *s){
    char ns[200];
    strcpy(&ns[0],s);
    char ns1[200];
    char nstr[200];
    char final[300] ;
    //char output[30]= "outputsdi1800040";       
    char slash[2]="/";
    int nstrcount=0,u=0;

    

	revstr(ns);
	nstrcount = strcspn(ns, "/");		
	strncpy(nstr, ns, nstrcount);
	nstr[nstrcount] = '\0'; 
	revstr(nstr);
	revstr(ns);
	ns[strlen(ns)-nstrcount-1] = '\0';

    sprintf(final, "%s", ns);
    
    
    struct stat sb;

    if (!(stat(final, &sb) == 0 && S_ISDIR(sb.st_mode))) {
       // printf("YES\n");
        createdirectory(final);
        remove(final);
        int result = mkdir(final, 0777);
        
    }
}
