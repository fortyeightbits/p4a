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
	pthread_mutex_lock(&pageQueueMutex);
    P_Node_t* temp = (P_Node_t*)malloc(sizeof(P_Node_t));
    //temp->data = (char*)malloc(1000*sizeof(char));
    //temp->link = (char*)malloc(100*sizeof(char));
	temp->next = NULL;
	temp->data = NULL;
	temp->link = NULL;
	q->head = temp;
	q->tail = temp;	
	pthread_mutex_unlock(&pageQueueMutex);
}

//for downloader to enqueue page
int PageQueue_enqueue(char* x, char* link, P_Queue_t* q) {
	printf("%d: enqueue getting lock\n", pthread_self());
	pthread_mutex_lock(&pageQueueMutex);
	printf("%d: enqueue got lock\n", pthread_self());
	if(q->head->data == NULL){
	q->tail->data = (char*)malloc(1000*sizeof(char));
    q->tail->link = (char*)malloc(100*sizeof(char));
	strcpy(q->tail->data, x);
	strcpy(q->tail->link, link);	
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
	//printf("%d: enqueue signalling\n", pthread_self());
	pthread_cond_signal(&pageQueueFill);
			//
	printf("%d: enqueue-----qhead data: %s\n", pthread_self(), q->head->data);
	//
	//printf("%d: enqueue going to release lock\n", pthread_self());
	pthread_mutex_unlock(&pageQueueMutex);
	printf("%d: enqueue released lock\n", pthread_self());
	return 0;
}
// TODO: Look into returnvalue and link to see if malloc is required.
int PageQueue_dequeue(P_Queue_t *q, char** returnvalue, char** link) {
	printf("%d: pagedequeue getting lock\n", pthread_self());
	pthread_mutex_lock(&pageQueueMutex);
	printf("%d: pagedequeue got lock\n", pthread_self());
    while (q->head->data == NULL){ //sleep if there's no pages to get
        printf("%d: pagedequeue sleeping\n", pthread_self());
		pthread_cond_wait(&pageQueueFill, &pageQueueMutex);
	}
	//printf("%d: pagedequeue woke up\n", pthread_self());
		//
	printf("%d: before dequeue-----qhead data: %s\n", pthread_self(), q->head->data);
	
	//
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
		q->head->data = NULL; //added this: do i need it for linkqueue?
			printf("%d: after dequeue (-1)-----qhead data: %s\n", pthread_self(), q->head->data);
		//printf("%d: pagedequeue going to release lock\n", pthread_self());
		pthread_mutex_unlock(&pageQueueMutex);
		printf("%d: pagedequeue release lock (-1)\n", pthread_self());
		return -1; // queue was empty
	}
  	q->head = newHead;
    free(tmp->data);
    free(tmp->link);
	free(tmp);
		printf("%d: after dequeue-----qhead data: %s\n", pthread_self(), q->head->data);
	pthread_mutex_unlock(&pageQueueMutex);
	printf("%d: pagedequeue release lock\n", pthread_self());
	return 0;
}
