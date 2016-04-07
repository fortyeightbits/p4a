#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <pthread.h>

//for parser to enqueue 
/*typedef struct EnQArgPacket{
	char* x;
	Queue_t* q;
} EPacket_t;

//for downloader to dequeue
typedef struct DeQArgPacket{
	Queue_t* q;
	char** returnvalue;
} DPacket_t;*/

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
int queue_size = 2; //TODO: PASS IN AS ARG
  
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
		q->size = 0;
		Node_t* temp = (Node_t*)malloc(sizeof(Node_t));	
		temp->next = NULL;
		q->head = temp;
		q->tail = temp;	
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

	Queue_t linkQueue;
	void* downloader(void *arg);
	void* parser(void *arg);
	

int main(int argc, char* argv[])
{
	
	
	Queue_init(&linkQueue);
	char* text = "link1";
	char* text2 = "link2";
	Queue_enqueue(text, &linkQueue);
	Queue_enqueue(text2, &linkQueue);
	
	pthread_t downloadthread;
	pthread_t parsethread;
	pthread_create(&downloadthread, NULL, downloader, (void *)&linkQueue);
	pthread_create(&parsethread, NULL, parser, (void *)&linkQueue);
	pthread_join(downloadthread, NULL);
	pthread_join(parsethread, NULL);
}


void* downloader(void* q) {

	//downloader dequeues from link queue
	
	char* returnvalue = (char*)malloc(sizeof(char));
	
	//while(linkQueue.size != 0){
    printf("downloading page!\n"); 	
	Queue_dequeue(&q, &returnvalue);
	printf("return: %s\n", returnvalue);
	//}
	return NULL;
}

void* parser(void* arg) {
	//parser enqueues to link queue
	
	 printf("parsing page!\n"); 
	char* text3 = "link3";
	Queue_enqueue(text3, &arg);
	
	return NULL;
}