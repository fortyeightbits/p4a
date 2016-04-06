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
	int size;
} Queue_t;
  
pthread_mutex_t linkQueueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t linkQueueEmpty = PTHREAD_COND_INITIALIZER;
pthread_cond_t linkQueueFill = PTHREAD_COND_INITIALIZER;
int queue_size = 2;
  
void Queue_init(Queue_t* q)
{
	pthread_mutex_lock(&linkQueueMutex);
	//////////
	q->size = 0;
	Node_t* temp = (Node_t*)malloc(sizeof(Node_t));	
	temp->next = NULL;
	q->head = temp;
	q->tail = temp;	
	//////////
	pthread_mutex_unlock(&linkQueueMutex);
}

//linkqueue enqueued by parser. Can only be enqueued if not full
int Queue_enqueue(char* x, Queue_t* q) {
	
	pthread_mutex_lock(&linkQueueMutex);
	while (q->size == queue_size) //TODO: get queue size
        pthread_cond_wait(&linkQueueEmpty, &linkQueueMutex);
	//////////////////////////
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
	//////////////////////////
	pthread_cond_signal(&linkQueueFill);
	pthread_mutex_unlock(&linkQueueMutex);
	return 0;
}

//downloader dequeues from link queue. Only dequeue if not empty
int Queue_dequeue(Queue_t *q, char** returnvalue) {
	
	pthread_mutex_lock(&linkQueueMutex);
	while (q->size == 0) //sleep if there's no links to get
        pthread_cond_wait(&linkQueueFill, &linkQueueMutex);
	/////////////////
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
	q->size--;
	/////////////////////////
	pthread_cond_signal(&linkQueueEmpty);
	pthread_mutex_unlock(&linkQueueMutex);
	
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

	
	void* downloader(void *arg);
	//void* parser(void *arg);


int main(int argc, char* argv[])
{
	
	Queue_t linkQueue;
	Queue_init(&linkQueue);
	Queue_t pageQueue;
	Queue_init(&pageQueue);
	
	pthread_t downloadthread;
	//pthread_t parsethread;
	if (pthread_create(&downloadthread, NULL, downloader, (void *)&linkQueue) != 0)
		printf("Threading sucks..");
	//pthread_create(&parsethread, NULL, parser, (void *)&pageQueue);
	pthread_join(downloadthread, NULL);
	
	/*
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
	printf("%s\n", returnvalue);
	*/
}

void* downloader(void* q) {
	Queue_t* qq  = q;
	char* returnvalue = (char*)malloc(sizeof(char));
	
    printf("downloading page!\n"); 
	char* text = "meow";
	if (Queue_enqueue(text, qq) != 0)
	//NOTE: Enqueue works (according to gdb, see I actually use it okayy)
		printf("enqueue failed\n");
	
	//NOTE: Dequeue doesn't work. It gets stuck D:
	if (Queue_dequeue(qq, &returnvalue) != 0)
		printf("dequeue failed\n");
	
	return NULL;
}

void* parser(void* q) {
	Queue_t* qq  = q;
    printf("parsing page!\n"); 
	char* text2 = "nuu";
	Queue_enqueue(text2, qq);
	return NULL;
}