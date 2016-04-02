#include "cs537.h"


// TYPDEFINES LISTED HERE
typedef struct Node {
	char* data;
	struct Node* next;
} Node_t;

typedef struct Queue {
	Node_t *head;
	Node_t *tail;
} Queue_t;

// FUNCTION FORWARD DECLARATIONS HERE
void Queue_init(Queue_t *);
void Queue_enqueue(char* , Queue_t* );
int Queue_dequeue(Queue_t *, char* );
