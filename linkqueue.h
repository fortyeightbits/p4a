#include "cs537.h"


// TYPDEFINES LISTED HERE
typedef struct L_Node {
	char* data;
	struct Node* next;
} L_Node_t;

typedef struct L_Queue {
    L_Node_t *head;
    L_Node_t *tail;
	int size;
} L_Queue_t;
  
typedef struct L_EnQArgPacket{
	char* x;
	L_Queue_t* q;
	int queue_size;
} L_EPacket_t;

typedef struct L_DeQArgPacket{
	L_Queue_t* q;
	char** returnvalue;
} L_DPacket_t;

// FUNCTION FORWARD DECLARATIONS HERE
void LinkQueue_init(L_Queue_t* q);
int LinkQueue_enqueue(char* x, L_Queue_t* q, int queue_size) ;
int LinkQueue_dequeue(L_Queue_t *q, char** returnvalue);
