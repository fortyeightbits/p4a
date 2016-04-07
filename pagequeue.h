#include "cs537.h"


// TYPDEFINES LISTED HERE
typedef struct P_Node {
	char* data;
	struct P_Node* next;
} P_Node_t;

typedef struct P_Queue {
	Node_t *head;
	Node_t *tail;
} P_Queue_t;
  
typedef struct EnQArgPacket{
	char* x;
	P_Queue_t* q;
} EPacket_t;

typedef struct DeQArgPacket{
	P_Queue_t* q;
	char** returnvalue;
} DPacket_t;

// FUNCTION FORWARD DECLARATIONS HERE
void PageQueue_init(P_Queue_t* q);
int PageQueue_enqueue(char* x, P_Queue_t* q);
int PageQueue_dequeue(P_Queue_t *q, char** returnvalue);
