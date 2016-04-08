#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <pthread.h>
#include "linkqueue.h"

pthread_mutex_t linkQueueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t linkQueueEmpty = PTHREAD_COND_INITIALIZER;
pthread_cond_t linkQueueFill = PTHREAD_COND_INITIALIZER;

void LinkQueue_init(L_Queue_t* q){
	printf("%d: init getting lock\n", pthread_self());
	pthread_mutex_lock(&linkQueueMutex);
	q->size = 0;
    L_Node_t* temp = (L_Node_t*)malloc(sizeof(L_Node_t));
    temp->data = (char*)malloc(100*sizeof(char));
	temp->next = NULL;
	q->head = temp;
	q->tail = temp;	
	pthread_mutex_unlock(&linkQueueMutex);
	printf("%d: init released lock\n", pthread_self());
}

//for parser to enqueue links
int LinkQueue_enqueue(char* x, L_Queue_t* q, int queue_size) {
	
	printf("%d: enqueue getting lock\n", pthread_self());
	pthread_mutex_lock(&linkQueueMutex);
	printf("%d: eequeue got lock\n", pthread_self());
	while (q->size == queue_size){ 
        printf("%d: enqueue sleeping because queue is full\n", pthread_self());
		pthread_cond_wait(&linkQueueEmpty, &linkQueueMutex);
	}
	printf("%d: enqueue either woke up or never slept\n", pthread_self());
	if(q->head->data == NULL){
        strcpy(q->tail->data, x);
	}
	else{
        L_Node_t* temp = (L_Node_t*)malloc(sizeof(L_Node_t));
        temp->data = (char*)malloc(100*sizeof(char));
        strcpy(temp->data, x);
		temp->next = NULL;

		q->tail->next = temp;
		q->tail = temp;
	}
	q->size++;
	printf("%d: enqueue signalling\n", pthread_self());
	pthread_cond_signal(&linkQueueFill);
	printf("%d: enqueue going to release lock\n", pthread_self());
	pthread_mutex_unlock(&linkQueueMutex);
	printf("%d: enqueue released lock\n", pthread_self());
	return 0;
}

//for downloader to dequeue links
int LinkQueue_dequeue(L_Queue_t *q, char** returnvalue) {
	printf("%d: dequeue getting lock\n", pthread_self());
	pthread_mutex_lock(&linkQueueMutex);
	printf("%d: dequeue got lock\n", pthread_self());
	while (q->size == 0){ //sleep if there's no links to get
        printf("%d: dequeue sleeping\n", pthread_self());
		pthread_cond_wait(&linkQueueFill, &linkQueueMutex);
	}
	printf("%d: dequeue woke up\n", pthread_self());
    L_Node_t *tmp = q->head;
    strcpy(*returnvalue, q->head->data);
    L_Node_t *newHead = tmp->next;
	if (newHead == NULL) {
        free(q->head->data);
		free(q->head);
		q->size = 0;
        L_Node_t* temp = (L_Node_t*)malloc(sizeof(L_Node_t));
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
    free(tmp->data);
	free(tmp);
	q->size--;
	printf("%d: dequeue signalling\n", pthread_self());
	pthread_cond_signal(&linkQueueEmpty);
	pthread_mutex_unlock(&linkQueueMutex);
	printf("%d: dequeue release lock\n", pthread_self());
	return 0;
}
