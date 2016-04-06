#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <pthread.h>

// Header file section:
#define CONSUMERS 2
#define PRODUCERS 2

pthread_mutex_t linkQueueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t	mainMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t linkQueueEmpty = PTHREAD_COND_INITIALIZER;
pthread_cond_t linkQueueFill = PTHREAD_COND_INITIALIZER;
pthread_cond_t noWorkInSystem = PTHREAD_COND_INTIALIZER;
int workInSystem = 0

typedef struct Node {
	char* data;
	struct Node* next;
} Node_t;

typedef struct Queue {
	Node_t *head;
	Node_t *tail;
	int size;
} Queue_t;
  
typedef struct EnQArgPacket{
	char* x;
	Queue_t* q;
	int queue_size;
} EPacket_t;

typedef struct DeQArgPacket{
	Queue_t* q;
	char** returnvalue;
	int queue_size;
} DPacket_t;

//---------------------------------------------------------


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

//value has head->next's value, not the head
int Queue_dequeue(Queue_t *q, char** returnvalue) {
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
  	q->head = newHead;
	free(tmp);
	q->size--;
	/////////////////////////
	pthread_cond_signal(&linkQueueEmpty);
	pthread_mutex_unlock(&linkQueueMutex);
	
	return 0;
}


void enq_helper(void* argPtr)
{
	Queue_enqueue((Epacket_t*)argPtr->x, (Epacket_t*)argPtr->q);
}

void deq_helper(void* argPtr)
{
	Queue_dequeue((DPacket_t*)argPtr->q,(DPacket_t*)argPtr->returnvalue);
}

int main(int argc, char* argv[])
{
	pthread_t consumer_pool[10];
	pthread_t producer_pool[10];
	Epacket_t producerArgs[10];
	Dpacket_t consumerArgs[10];
	char* dataArray[10] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j"};
	
	char* returnValuePool[10];
	void (*enqptr)(void* ) = &enq_helper;
	void (*deqptr)(void* ) = &deq_helper;
	Queue_t kiutqueue;
	char* text = "meow";
	char* text2 = "nuu";
	Queue_init(&kiutqueue);
	int queue = 2; //TODO: PASS IN AS ARG
	
	// Initialization of consumer and producer data packets -------------------
	int i;
	for(i = 0;i < CONSUMERS; i++)
	{
		consumerArgs[i].q = &kiutqueue;
		consumerArgs[i].returnvalue = &returnValuePool[i];
		consumerArgs[i].queue_size = queue;
	}
	
	int j;
	for(j = 0;j < PRODUCERS; j++)
	{
		producerArgs[j].q = &kiutqueue;
		producerArgs[j].x = dataArray[j];
		producerArgs[j].queue_size = queue;
	}
	
	// ---------------------------------------------------------------------------
	
	int k;
	for(k = 0; k < PRODUCERS; k++)
	{
		pthread_create(&producer_pool[k], NULL, enqptr, producerArgs[k]);
		pthread_create(&consumer_pool[k], NULL, deqptr, consumerArgs[k]);				
	}
	
	pthread_mutex_lock(&mainMutex);
	while (workInSystem == 0 && kiutqueue.size == 0)
		pthread_cond_wait(&noWorkInSystem,&mainMutex);
	pthread_mutex_unlock(&mainMutex);
	// Signalling for this condition has yet to be done Buggle, please look into it.	

}
		
		
	/*Queue_enqueue(text, &kiutqueue);
	Queue_enqueue(text2, &kiutqueue);
	Queue_dequeue(&kiutqueue, &returnValue);
	Queue_dequeue(&kiutqueue, &returnValue1);
	printf("%s\n", (kiutqueue.head)->data);
	printf("returndata: %s\n", returnValue);*/
	//The printf below SHOULD seg fault buggle, so set breakpoint before(hehe u have to use gdb now)
	//printf("%s\n", (kiutqueue.head)->next->data);
	//printf("%s\n", returnvalue);
