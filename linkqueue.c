// Header File include:
#include "linkqueue.h"

void Queue_init(Queue_t* q)
{
	q->size = 0;
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
	q->size++;
}

//value has head->next's value, not the head
int Queue_dequeue(Queue_t *q) {
    Node_t *tmp = q->head;
    Node_t *newHead = tmp->next;
    if (newHead == NULL) {
        return -1; // queue was empty       
    }
//	value = newHead->data;
    q->head = newHead;
    free(tmp);
	q->size--;
    return 0;
}

