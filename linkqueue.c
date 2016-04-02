// Header File include:
#include "linkqueue.h"

void Queue_init(Queue_t *q) {
	Node_t *tmp = malloc(sizeof(Node_t));
	tmp->next = NULL;	
  	q->head = q->tail = tmp;
}
  
void Queue_enqueue(char* x, Queue_t* q) {
  	Node_t* temp = (Node_t*)malloc(sizeof(Node_t));
  	temp->data = x; 
  	temp->next = NULL;
  	q->tail->next = tmp;
  	q->tail = tmp;
}

//value has head->next's value, not the head
int Queue_dequeue(Queue_t *q, char* value) {
	Node_t *tmp = q->head;
	Node_t *newHead = tmp->next;
	if (newHead == NULL) {
		return -1; // queue was empty
	}
	*value = newHead->value;
  	q->head = newHead;
	free(tmp);
	return 0;
}
