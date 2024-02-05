#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>



typedef struct NODE_PAYLOAD_S
{
   /* Data Payload (defined by coder) */
   char name[70];
	   
} NODE_PAYLOAD_T;

typedef struct LIST_NODE_S
{
    /* Next-node pointer */
    struct LIST_NODE_S *next;     /* pointer to the next node in the list. */
    NODE_PAYLOAD_T      payload;  /* Data Payload (defined by coder) */
} LIST_NODE_T;
	
LIST_NODE_T *listHead = NULL;
	
int LIST_InsertHeadNode(LIST_NODE_T **, char *);
int PrintListPayloads(LIST_NODE_T *);
int LIST_GetTailNode( LIST_NODE_T  *I__listHead,  LIST_NODE_T **_O_listTail );
int LIST_InsertTailNode(LIST_NODE_T **IO_head, char  *I__name);
int LIST_FetchParentNodeByName( LIST_NODE_T *I__head,const char  *I__name,LIST_NODE_T **_O_parent );
int LIST_InsertNodeByName(LIST_NODE_T **IO_head,char  *I__name);
int LIST_FetchNodeByName( LIST_NODE_T  *I__head,const char   *I__name, LIST_NODE_T **_O_node,LIST_NODE_T **_O_parent );
int LIST_DeleteNodeByName( LIST_NODE_T **IO_head,char  *I__name);
int simaia=0;
	
int LIST_InsertHeadNode(LIST_NODE_T **IO_head,char *I__name)
{
   int rCode=0;
   LIST_NODE_T *newNode = NULL;

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
   
   
   
int PrintListPayloads(LIST_NODE_T *head)
{
   int rCode=0;
   LIST_NODE_T *cur = head;
   int nodeCnt=0;
   printf("[");
   while(cur)
      {
      ++nodeCnt;
      printf("%s\n",
            cur->payload.name
            );
       cur=cur->next;
       }
	printf("]");
    printf("%d nodes printed.\n", nodeCnt);

   return(rCode);
}
   
  
  



 int LIST_GetTailNode(
         LIST_NODE_T  *I__listHead,   /* The caller supplied list head pointer. */
         LIST_NODE_T **_O_listTail    /* The function sets the callers pointer to the
                                         last node. */
         )
{
      int rCode=0;
      LIST_NODE_T *curNode = I__listHead;

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
	  
	  
int LIST_InsertTailNode(
      LIST_NODE_T **IO_head,
      char         *I__name

    )
{
   int rCode=0;
   LIST_NODE_T *tailNode;
   LIST_NODE_T *newNode = NULL;

   /* Get a pointer to the last node in the list. */
   rCode=LIST_GetTailNode(*IO_head, &tailNode);
   if(rCode)
      {
      fprintf(stderr, "LIST_GetTailNode() reports: %d\n", rCode);
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
   
   
   
int LIST_FetchParentNodeByName( 
      LIST_NODE_T *I__head,
      const char  *I__name,
      LIST_NODE_T **_O_parent
      )
{
   int rCode=0;
   LIST_NODE_T *parent = NULL;
   LIST_NODE_T *curNode = I__head;

   /* Inform the caller of an 'empty list' condition. */
   if(NULL == I__head)
      {
      rCode=ENOENT;
      goto CLEANUP;
      }

   /* Find a node with a payload->name string greater than the I__name string */
   while(curNode)
      {
      if(strcmp(curNode->payload.name, I__name) > 0)
         break;

      parent = curNode; /* Remember this node. It is the parent of the next node. */
      curNode=curNode->next;  /* On to the next node. */
      }

   /* Set the caller's 'parent' pointer. */
   if(_O_parent)
      *_O_parent = parent;

   CLEANUP:

   return(rCode);
}
   


   
int LIST_InsertNodeByName(
      LIST_NODE_T **IO_head,
      char         *I__name
      )
{
   int rCode=0;
   LIST_NODE_T *parent;
   LIST_NODE_T *newNode = NULL;

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

   /* Find the proper place to link this node */
   rCode=LIST_FetchParentNodeByName(*IO_head, I__name, &parent);
   switch(rCode)
      {
      case 0:
         break;

      case ENOENT:
         /* Handle empty list condition */ 
         newNode->next = NULL;
         *IO_head = newNode;
         rCode=0;
         goto CLEANUP;

      default:
         fprintf(stderr, "LIST_FetchParentNodeByName() reports: %d\n", rCode);
         goto CLEANUP;
      }
   
     /* Handle the case where all current list nodes are greater than the new node. */
   /* (Where the new node will become the new list head.) */
   if(NULL == parent)
      {
      newNode->next = *IO_head;
      *IO_head = newNode;
      goto CLEANUP;
      }

   /* Final case, insert the new node just after the parent node. */
   newNode->next = parent->next;
   parent->next = newNode;

   CLEANUP:

   return(rCode);
}
    

   
int LIST_FetchNodeByName( 
      LIST_NODE_T  *I__head,
      const char   *I__name,
      LIST_NODE_T **_O_node,
      LIST_NODE_T **_O_parent
      )
{
   int rCode=0;
   LIST_NODE_T *parent = NULL;
   LIST_NODE_T *curNode = I__head;

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
   


int LIST_DeleteNodeByName(
      LIST_NODE_T **IO_head,
      char         *I__name
      )
{
   int rCode=0;
   LIST_NODE_T *parent;
   LIST_NODE_T *delNode = NULL;

   /* Find the node to delete. */
   rCode=LIST_FetchNodeByName(*IO_head, I__name, &delNode, &parent); 
   switch(rCode)
   {
      case 0:
         break;

      case ENOENT:
         fprintf(stderr, "Matching node not found.\n");
         goto CLEANUP;

      default:
         fprintf(stderr, "LIST_FetchNodeByName() reports: %d\n", rCode);
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
	



int LISTsearchByName( 
      LIST_NODE_T  *I__head,
      const char   *I__name
      )
{
   int rCode=0;
   LIST_NODE_T *parent = NULL;
   LIST_NODE_T *curNode = I__head;

   /* Search the list for a matching payload name. */
   while(curNode)
   {
      if(0 == strcmp(curNode->payload.name, I__name))		  
	   { 
	   simaia=1;
	   goto CLEANUP;
	   //return(rCode);
	   }
      parent = curNode;   /*this node will be the parent of the next. */
      curNode=curNode->next;
   }

   /* If no match is found, inform the caller. */
   if(NULL == curNode)
   {
	 simaia=0;	 
     rCode=ENOENT;
     goto CLEANUP;
   }
	 
   CLEANUP:

   return(rCode);
}


