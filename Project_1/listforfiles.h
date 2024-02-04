#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#define NUM_THREADS 5

typedef struct NODE_NAME_S
{
   char name[500];
 } NODE_NAME_T;

typedef struct LIST_NAME_S
{
   /* Next-node pointer */
   struct LIST_NAME_S *next;     /* pointer to the next node in the list. */
   NODE_NAME_T      payload;  /* Data Payload (defined by coder) */
} LIST_NAME_T;
	
LIST_NAME_T *Headname = NULL;
int sim=0;
	
int LIST_InsertHeadNodeName(LIST_NAME_T **IO_head,char *I__name)
 {
   int rCode=0;
   LIST_NAME_T *newNode = NULL;

   /* Allocate memory for new node (with its payload). */
   newNode=malloc(sizeof(*newNode));
   if(NULL == newNode)
   {
      rCode=ENOMEM;   /* ENOMEM is defined in errno.h */
      fprintf(stderr, "malloc() failed.\n");
      goto CLEANUP;
	}

   /* Initialize the new node's payload. */       
   snprintf(newNode->payload.name, sizeof(newNode->payload.name), "%s", I__name);

   /* Link this node into the list as the new head node. */
   newNode->next = *IO_head;
   *IO_head = newNode;

   CLEANUP:
   return(rCode);
 }
   
 
int PrintListPayloadsName(LIST_NAME_T *head)
{
   int rCode=0;
   LIST_NAME_T *cur = head;
   int nodeCnt=0;
   while(cur)
   {
      ++nodeCnt;
      printf("%s\n",
            cur->payload.name
            );
       cur=cur->next;
    }
    printf("%d nodes printed.\n", nodeCnt);
   return(rCode);
}
	
long int threads_id[70];

void numberof(int until)
{
	srand(time(0));
	int i;
	long int number=140430776;
	char final[30];
	long long int final1;
	char *ptr;
	long ret;
	for (i = 0; i <= until; i++) {
		long int num = (rand() %1000000 + 1000000);
		sprintf(final, "%ld%ld", number,num);
		threads_id[i] = strtol(final, &ptr, 10);
	}
}
	
int LIST_GetTailNodeNames(
         LIST_NAME_T  *I__listHead,   /* The caller supplied list head pointer. */
         LIST_NAME_T **_O_listTail    /* The function sets the callers pointer to the last node. */
         )
{
      int rCode=0;
      LIST_NAME_T *curNode = I__listHead;

      /* Iterate through all list nodes until the last node is found. */
      /* The last node's 'next' field, which is always NULL. */
      if(curNode)
         {
         while(curNode->next)
            curNode=curNode->next;
         }

      /* Set the caller's pointer to point to the last (ie: tail) node. */
      if(_O_listTail)
         *_O_listTail = curNode;

      return(rCode);
}
	  
	  
int LIST_InsertTailNodeNames(
      LIST_NAME_T **IO_head,
      char         *I__name
      )
{
   int rCode=0;
   LIST_NAME_T *tailNode;
   LIST_NAME_T *newNode = NULL;

   /* Get a pointer to the last node in the list. */
   rCode=LIST_GetTailNodeNames(*IO_head, &tailNode);
   if(rCode)
   {
      fprintf(stderr, "LIST_GetTailNodeNames() reports: %d\n", rCode);
      goto CLEANUP;
   }
	  
	/* Allocate memory for new node (with its payload). */
   newNode=malloc(sizeof(*newNode));
   if(NULL == newNode)
   {
      rCode=ENOMEM;   /* ENOMEM is defined in errno.h */
      fprintf(stderr, "malloc() failed.\n");
      goto CLEANUP;
   }

   /* Initialize the new node's payload. */       
   snprintf(newNode->payload.name, sizeof(newNode->payload.name), "%s", I__name);

   /* Link this node into the list as the new tail node. */
   newNode->next = NULL;
   if(tailNode)
      tailNode->next = newNode;
   else
	  *IO_head = newNode;

	CLEANUP:

   return(rCode);
}
   
   
   
 int LISTsearchNameNodes( 
      LIST_NAME_T  *I__head,
      const char   *I__name
      )
   {
   int rCode=0;
   LIST_NAME_T *parent = NULL;
   LIST_NAME_T *curNode = I__head;

   /* Search the list for a matching payload name. */
   while(curNode)
      {
      if(0 == strcmp(curNode->payload.name, I__name))		  
	    { 
		 sim=1;
		 return(rCode);
	   }
      parent = curNode;   /*this node will be the parent of the next. */
      curNode=curNode->next;
      }

   /* If no match is found, inform the caller. */
   if(NULL == curNode)
     {
     rCode=ENOENT;
	 sim=0;
     goto CLEANUP;
     }
	 
	CLEANUP:

   return(rCode);
}
   

   
int LIST_FetchNodeByName1( 
      LIST_NAME_T  *I__head,
      const char   *I__name,
      LIST_NAME_T **_O_node,
      LIST_NAME_T **_O_parent
      )
{
   int rCode=0;
   LIST_NAME_T *parent = NULL;
   LIST_NAME_T *curNode = I__head;

   /* Search the list for a matching payload name. */
   while(curNode)
      {
      if(0 == strcmp(curNode->payload.name, I__name))
         break;

      parent = curNode;   /* Remember this node; it will be the parent of the next. */
      curNode=curNode->next;
      }

   /* If no match is found, inform the caller. */
   if(NULL == curNode)
     {
     rCode=ENOENT;
     goto CLEANUP;
     }

   /* Return the matching node to the caller. */
   if(_O_node)
      *_O_node = curNode;

   /* Return parent node to the caller. */
   if(_O_parent)
      *_O_parent = parent;

	CLEANUP:

   return(rCode);
 }
   
   
   
int LIST_DeleteNodeByNameCitizen(
      LIST_NAME_T **IO_head,
      char         *I__name
      )
{
   int rCode=0;
   LIST_NAME_T *parent;
   LIST_NAME_T *delNode = NULL;

   /* Find the node to delete. */
   rCode=LIST_FetchNodeByName1(*IO_head, I__name, &delNode, &parent); 
   switch(rCode)
      {
      case 0:
         break;

      case ENOENT:
         fprintf(stderr, "Matching node not found.\n");
         goto CLEANUP;

      default:
         fprintf(stderr, "LIST_FetchNodeByName1() reports: %d\n", rCode);
         goto CLEANUP;
      }
      /* Unlink the delNode from the list. */
   if(NULL == parent)
      *IO_head = delNode->next;
   else
      parent->next = delNode->next;

   /* Free the delNode and its payload. */
   free(delNode);

	CLEANUP:

   return(rCode);
}        


void FreeListNodes(LIST_NAME_T *head) {
    LIST_NAME_T *current = head;
    LIST_NAME_T *next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
}
