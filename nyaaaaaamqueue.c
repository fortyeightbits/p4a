#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <pthread.h>

typedef struct Node {
	char* data;
	struct Node* next;
} Node_t;

typedef struct Queue {
	Node_t *head;
	Node_t *tail;
} Queue_t;
  
void Queue_init(Queue_t* q)
{
	Node_t* temp = (Node_t*)malloc(sizeof(Node_t));	
	temp->next = NULL;
	q->head = temp;
	q->tail = temp;	
}
void Queue_enqueue(char* x, Queue_t* q) {
	if(q->head->data == NULL)
	{
		q->tail->data = x;		
	}
	else{
		Node_t* temp = (Node_t*)malloc(sizeof(Node_t));
		temp->data = x; 
		temp->next = NULL;

		q->tail->next = temp;
		q->tail = temp;
	}
}

//value has head->next's value, not the head
int Queue_dequeue(Queue_t *q, char** returnvalue) {
	Node_t *tmp = q->head;
	*returnvalue = q->head->data;
	Node_t *newHead = tmp->next;
	if (newHead == NULL) {
		free(q->head);
		Queue_init(q);
		return -1; // queue was empty
	}
//	value = newHead->data;
  	q->head = newHead;
	free(tmp);
	return 0;
}


/*int main(int argc, char* argv[])
{
	Queue_t kiutqueue;
	char* text = "meow";
	char* text2 = "bugglecute";
	char* text3 = "manjabuggle";
	char* returnvalue = (char*)malloc(sizeof(char));
	Queue_enqueue(text, &kiutqueue);
	Queue_enqueue(text2, &kiutqueue);
	Queue_enqueue(text3, &kiutqueue);
	Queue_dequeue(&kiutqueue, returnvalue);
	Queue_dequeue(&kiutqueue, returnvalue);	
	printf("%s\n", (kiutqueue.head)->data);
}*/

int main(int argc, char* argv[])
{
	char* returnValue = NULL;
	char* returnValue1 = NULL;
	Queue_t kiutqueue;
	char* text = "meow";
	char* text2 = "nuu";
	Queue_init(&kiutqueue);
	Queue_enqueue(text, &kiutqueue);
	Queue_enqueue(text2, &kiutqueue);
	Queue_dequeue(&kiutqueue, &returnValue);
	Queue_dequeue(&kiutqueue, &returnValue1);
	printf("%s\n", (kiutqueue.head)->data);
	printf("returndata: %s\n", returnValue);
	//The printf below SHOULD seg fault buggle, so set breakpoint before(hehe u have to use gdb now)
	printf("%s\n", (kiutqueue.head)->next->data);
	//printf("%s\n", returnvalue);
}
