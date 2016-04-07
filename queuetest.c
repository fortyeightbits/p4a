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
pthread_cond_t noWorkInSystem = PTHREAD_COND_INITIALIZER;
int workInSystem = 0;
//ALSO: atm each thread only dequeues once. in crawler it should do while queue not empty. I think.

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
} DPacket_t;

//---------------------------------------------------------


void Queue_init(Queue_t* q){
	printf("%d: init getting lock\n", pthread_self());
	pthread_mutex_lock(&linkQueueMutex);
	//////////
	q->size = 0;
	Node_t* temp = (Node_t*)malloc(sizeof(Node_t));	
	temp->next = NULL;
	q->head = temp;
	q->tail = temp;	
	//////////
	pthread_mutex_unlock(&linkQueueMutex);
	printf("%d: init released lock\n", pthread_self());
}

int Queue_enqueue(char* x, Queue_t* q, int queue_size) {
	
	printf("%d: enqueue getting lock\n", pthread_self());
	pthread_mutex_lock(&linkQueueMutex);
	printf("%d: eequeue got lock\n", pthread_self());
	while (q->size == queue_size){ //TODO: get queue size
        printf("%d: enqueue sleeping because queue is full\n", pthread_self());
		pthread_cond_wait(&linkQueueEmpty, &linkQueueMutex);
	}
	//////////////////////////
	printf("%d: enqueue either woke up or never slept\n", pthread_self());
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
	printf("%d: enqueue signalling\n", pthread_self());
	pthread_cond_signal(&linkQueueFill);
	printf("%d: enqueue going to release lock\n", pthread_self());
	pthread_mutex_unlock(&linkQueueMutex);
	printf("%d: enqueue released lock\n", pthread_self());
	return 0;
}

//value has head->next's value, not the head
int Queue_dequeue(Queue_t *q, char** returnvalue) {
	printf("%d: dequeue getting lock\n", pthread_self());
	pthread_mutex_lock(&linkQueueMutex);
	printf("%d: dequeue got lock\n", pthread_self());
	while (q->size == 0){ //sleep if there's no links to get
        printf("%d: dequeue sleeping\n", pthread_self());
		pthread_cond_wait(&linkQueueFill, &linkQueueMutex);
	}
	/////////////////
	printf("%d: dequeue woke up\n", pthread_self());
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
		
		printf("%d: dequeue signalling(-1)\n", pthread_self());
		pthread_cond_signal(&linkQueueEmpty);
		printf("%d: dequeue going to release lock\n", pthread_self());
		pthread_mutex_unlock(&linkQueueMutex);
		printf("%d: dequeue release lock (-1)\n", pthread_self());
		return -1; // queue was empty
	}
  	q->head = newHead;
	free(tmp);
	q->size--;
	/////////////////////////
	printf("%d: dequeue signalling\n", pthread_self());
	pthread_cond_signal(&linkQueueEmpty);
	pthread_mutex_unlock(&linkQueueMutex);
	printf("%d: dequeue release lock\n", pthread_self());
	return 0;
}


//parser enqueues
void *enq_helper(void* argPtr)
{
	Queue_enqueue(((EPacket_t*)(argPtr))->x, ((EPacket_t*)(argPtr))->q, ((EPacket_t*)(argPtr))->queue_size);
	printf("%d: enqueuehelper getting mainmutex\n", pthread_self());
	pthread_mutex_lock(&mainMutex);
	printf("%d: enqueuehelper got mainmutex, ++work \n", pthread_self());
	workInSystem++;
	printf("%d: enqueuehelper going to release mainmutex\n", pthread_self());
	pthread_mutex_unlock(&mainMutex);
	printf("%d: enqueuehelper released mainmutex\n", pthread_self());
}

//downloader dequeues 
void *deq_helper(void* argPtr)
{
	Queue_dequeue(((DPacket_t*)argPtr)->q,((DPacket_t*)argPtr)->returnvalue);
	printf("%d: dequeuehelper getting mainmutex\n", pthread_self());
	pthread_mutex_lock(&mainMutex);
	printf("%d: dequeuehelper got mainmutex, --work \n", pthread_self());
	workInSystem--;
	if (workInSystem == 0){
		printf("%d: dequeuehelper waking main up, no more work\n", pthread_self());
		pthread_cond_signal(&noWorkInSystem);
	}
	printf("%d: dequeuehelper wakes, unlocking mainmutex\n", pthread_self());
	pthread_mutex_unlock(&mainMutex);
	printf("%d: dequeuehelper released mainmutex\n", pthread_self());
	//Remember that in the real crawler, workinsystem only incremented when something enqueued to linkqueue, dec when dequeued from pagequeue
}

int main(int argc, char* argv[])
{
	pthread_t consumer_pool[10];
	pthread_t producer_pool[10];
	EPacket_t producerArgs[10];
	DPacket_t consumerArgs[10];
	char* dataArray[10] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j"};
	
	char* returnValuePool[10];
	//void (*enqptr)(void* ) = &enq_helper;
	//void (*deqptr)(void* ) = &deq_helper;
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
		pthread_create(&producer_pool[k], NULL, enq_helper, ((void*)(&producerArgs[k])));
		pthread_create(&consumer_pool[k], NULL, deq_helper, ((void*)(&consumerArgs[k])));				
	}
	
	printf("main getting mainmutex lock\n");
	pthread_mutex_lock(&mainMutex);
	printf("main got mainmutex lock\n");
	
	while (workInSystem != 0 && kiutqueue.size != 0) {
		printf("main sleeping while waiting for 0 workinsystem\n");
		pthread_cond_wait(&noWorkInSystem,&mainMutex); //sleep while workers not done
	}
	printf("main woke up or never slept, is unlocking\n");
	pthread_mutex_unlock(&mainMutex);
	// Signalling for this condition has yet to be done Buggle, please look into it.	
	printf("main released mainmutex lock\n");
	
	int m;
	for(m = 0; m < PRODUCERS; m++)
	{
		pthread_join(producer_pool[m], NULL);
		pthread_join(consumer_pool[m], NULL);				
	}
	
	printf("done\n");
	return 0;
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
