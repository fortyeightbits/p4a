#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <pthread.h>
#include "pagequeue.h"

pthread_mutex_t pageQueueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t pageQueueFill = PTHREAD_COND_INITIALIZER;

void PageQueue_init(P_Queue_t* q){
	printf("%d: init getting lock\n", pthread_self());
	pthread_mutex_lock(&pageQueueMutex);
    P_Node_t* temp = (P_Node_t*)malloc(sizeof(P_Node_t));
    temp->data = (char*)malloc(1000*sizeof(char));
    temp->link = (char*)malloc(100*sizeof(char));
	temp->next = NULL;
	q->head = temp;
	q->tail = temp;	
	pthread_mutex_unlock(&pageQueueMutex);
	printf("%d: init released lock\n", pthread_self());
}

//for downloader to enqueue page
int PageQueue_enqueue(char* x, char* link, P_Queue_t* q) {
	printf("%d: enqueue getting lock\n", pthread_self());
	pthread_mutex_lock(&pageQueueMutex);
	printf("%d: eequeue got lock\n", pthread_self());
	if(q->head->data == NULL){
		q->tail->data = x;		
	}
	else{
        P_Node_t* temp = (P_Node_t*)malloc(sizeof(P_Node_t));
        temp->data = (char*)malloc(1000*sizeof(char));
        temp->link = (char*)malloc(100*sizeof(char));
        strcpy(temp->data, x);
        strcpy(temp->link, link);
		temp->next = NULL;

		q->tail->next = temp;
		q->tail = temp;
	}
	printf("%d: enqueue signalling\n", pthread_self());
	pthread_cond_signal(&pageQueueFill);
	printf("%d: enqueue going to release lock\n", pthread_self());
	pthread_mutex_unlock(&pageQueueMutex);
	printf("%d: enqueue released lock\n", pthread_self());
	return 0;
}
// TODO: Look into returnvalue and link to see if malloc is required.
int PageQueue_dequeue(P_Queue_t *q, char** returnvalue, char** link) {
	printf("%d: dequeue getting lock\n", pthread_self());
	pthread_mutex_lock(&pageQueueMutex);
	printf("%d: dequeue got lock\n", pthread_self());
	while (q->head == NULL){ //sleep if there's no pages to get
        printf("%d: dequeue sleeping\n", pthread_self());
		pthread_cond_wait(&pageQueueFill, &pageQueueMutex);
	}
	printf("%d: dequeue woke up\n", pthread_self());
    P_Node_t *tmp = q->head;
    strcpy(*returnvalue, q->head->data);
    strcpy(*link, q->head->link);
    P_Node_t *newHead = tmp->next;
	if (newHead == NULL) {
        free(q->head->data);
        free(q->head->link);
		free(q->head);
        P_Node_t* temp = (P_Node_t*)malloc(sizeof(P_Node_t));
		temp->next = NULL;
		q->head = temp;
		q->tail = temp;	
		printf("%d: dequeue going to release lock\n", pthread_self());
		pthread_mutex_unlock(&pageQueueMutex);
		printf("%d: dequeue release lock (-1)\n", pthread_self());
		return -1; // queue was empty
	}
  	q->head = newHead;
    free(tmp->data);
    free(tmp->link);
	free(tmp);
	pthread_mutex_unlock(&pageQueueMutex);
	printf("%d: dequeue release lock\n", pthread_self());
	return 0;
}
