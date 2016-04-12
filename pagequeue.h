#ifndef _PAGEQUEUE_H
#define _PAGEQUEUE_H

// TYPDEFINES LISTED HERE
typedef struct P_Node {
	char* data;
    char* link;
	struct P_Node* next;
} P_Node_t;

typedef struct P_Queue {
    P_Node_t *head;
    P_Node_t *tail;
    int size;
} P_Queue_t;
  
typedef struct P_EnQArgPacket{
	char* x;
	P_Queue_t* q;
} P_EPacket_t;

typedef struct P_DeQArgPacket{
	P_Queue_t* q;
	char** returnvalue;
} P_DPacket_t;

// FUNCTION FORWARD DECLARATIONS HERE
void PageQueue_init(P_Queue_t* );
int PageQueue_enqueue(char* , char* , P_Queue_t* );
int PageQueue_dequeue(P_Queue_t *, char**, char**);
#endif
