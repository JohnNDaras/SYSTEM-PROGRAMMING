typedef struct NODE_IP_S
{
    /* Data Payload (defined by coder) */
    char name[1000];
    int count;
} NODE_IP_T;

typedef struct LIST_IP_S
{
    /* Next-node pointer */
    struct LIST_IP_S *next;     /* pointer to the next node in the list. */
    NODE_IP_T      ip;  /* Data Payload (defined by coder) */
} LIST_IP_T;
	
LIST_IP_T *listHeadIp = NULL;
	
int LIST_InsertHeadNodeIp(LIST_IP_T **, char *, int);
int PrintListPayloadsIp(LIST_IP_T *,FILE *);
int LIST_GetTailNodeIp( LIST_IP_T  *I__listHead,  LIST_IP_T **_O_listTail );
int LIST_InsertTailNodeIp(LIST_IP_T **IO_head, char  *I__name, int  I__count);
int LISTsearchByNameIp( LIST_IP_T  *I__head, const char   *I__name);
int LISTIncreaseCounterIp( LIST_IP_T  *I__head, const char   *I__name);
int simaiaip=0;

	
int LIST_InsertHeadNodeIp(LIST_IP_T **IO_head,char *I__name,int I__count)
{
   int rCode=0;
   LIST_IP_T *newNode = NULL;

   /* Allocate memory for new node (with its payload). */
   newNode=malloc(sizeof(*newNode));
   if(NULL == newNode)
   {
      rCode=ENOMEM;   /* ENOMEM is defined in errno.h */
      fprintf(stderr, "malloc() failed.\n");
      goto CLEANUP;
   }

   /* Initialize the new node's payload. */       
   snprintf(newNode->ip.name, sizeof(newNode->ip.name), "%s", I__name);
   newNode->ip.count = I__count;

   /* Link this node into the list as the new head node. */
   newNode->next = *IO_head;
   *IO_head = newNode;

   CLEANUP:

   return(rCode);
}
   

   
int PrintListPayloadsIp(LIST_IP_T *head, FILE *fp)
{
   int rCode=0;
   LIST_IP_T *cur = head;
   int nodeCnt=0;

   while(cur)
   {
      ++nodeCnt;
            
       fprintf(fp,"%s,  %d\n", 
            cur->ip.name,
            cur->ip.count
            );
       cur=cur->next;
    }

    printf("%d nodes printed.\n", nodeCnt);

   return(rCode);
}
   
   
   
   
   
 int LIST_GetTailNodeIp(
     LIST_IP_T  *I__listHead,   /* The caller supplied list head pointer. */
     LIST_IP_T **_O_listTail    /* The function sets the callers pointer to the
                                    last node. */
     )
{
      int rCode=0;
      LIST_IP_T *curNode = I__listHead;

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
	  
	  
int LIST_InsertTailNodeIp(
    LIST_IP_T **IO_head,
    char         *I__name,
    int           I__count
    )
{
   int rCode=0;
   LIST_IP_T *tailNode;
   LIST_IP_T *newNode = NULL;

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
   snprintf(newNode->ip.name, sizeof(newNode->ip.name), "%s", I__name);
   newNode->ip.count = I__count;

   /* Link this node into the list as the new tail node. */
   newNode->next = NULL;
   if(tailNode)
      tailNode->next = newNode;
   else
	  *IO_head = newNode;

   CLEANUP:

   return(rCode);
}
   
   
   
   
int LISTsearchByNameIp( 
    LIST_IP_T  *I__head,
    const char   *I__name
    )
{
   int rCode=0;
   LIST_IP_T *parent = NULL;
   LIST_IP_T *curNode = I__head;

   /* Search the list for a matching payload name. */
   while(curNode)
   {
      if(0 == strcmp(curNode->ip.name, I__name))		  
	   { 
	   simaiaip=1;
	   goto CLEANUP;
	   //return(rCode);
	   }
      parent = curNode;   /*this node will be the parent of the next. */
      curNode=curNode->next;
   }

   /* If no match is found, inform the caller. */
   if(NULL == curNode)
   {
	 simaiaip=0;	 
     rCode=ENOENT;
     goto CLEANUP;
   }
	 
   CLEANUP:

   return(rCode);
}
   
   
int LISTIncreaseCounterIp( 
    LIST_IP_T  *I__head,
    const char   *I__name
  )
 {
   int rCode=0;
   LIST_IP_T *parent = NULL;
   LIST_IP_T *curNode = I__head;

   /* Search the list for a matching payload name. */
   while(curNode)
   {
      if(0 == strcmp(curNode->ip.name, I__name))		  
	   { 
			curNode->ip.count++;
			goto CLEANUP;
	   }
      parent = curNode;   /*this node will be the parent of the next. */
      curNode=curNode->next;
   }

   /* If no match is found, inform the caller. */
   if(NULL == curNode)
   {	 
     rCode=ENOENT;
     goto CLEANUP;
   }
	 
   CLEANUP:

   return(rCode);
}
   








