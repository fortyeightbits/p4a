#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <pthread.h>
#include "linkqueue.c"

int crawl(char *start_url,
	  int download_workers,
	  int parse_workers,
	  int queue_size,
	  char * (*_fetch_fn)(char *url),
	  void (*_edge_fn)(char *from, char *to)) {

  //link queue for parser to queue, downloader to dequeue urls
	pthread_mutex_t linkQueueMutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t linkQueueEmpty = PTHREAD_COND_INITIALIZER;
	pthread_cond_t linkQueueFill = PTHREAD_COND_INITIALIZER;
	Queue_t linkQueue;
	Queue_init(&linkQueue);

	//downloader is the consumer of links
	void* downloader(void *arg) {
		Pthread_mutex_lock(&linkQueueMutex);
		while (linkQueue->size == 0) //sleep if there's no links to get
			Pthread_cond_wait(&linkQueueFill, &linkQueueMutex);
		downloadpage(); //TODO download function
		Pthread_cond_signal(&linkQueueEmpty);
		Pthread_mutex_unlock(&linkQueueMutex);
7 	}

	//parser is the producer of links
	void* parser(void *arg) {
		Pthread_mutex_lock(&linkQueueMutex);
		while (linkQueue->size == queue_size) //sleep if queue is full. Is this right?
			Pthread_cond_wait(&linkQueueEmpty, &linkQueueMutex);
		parseparge(linkQueue, start_url, (*_fetch_fn)(char *url)); //TODO: parse page
		Pthread_cond_signal(&linkQueueFill);
		Pthread_mutex_unlock(&linkQueueMutex);
	}
	
	free(page);

  return -1;
}

//BUGGLE!!! Not sure if I'm using callbacks/functionptr(?) right. :( Trying to pass _fetch_fn to this parsepage function.
//
//fetches page and gets links, enqueues it to link queue
parsepage(Queue_t* linkqueue, char *start, char * (*_fetch_fn)(char *url)){
	char** linkArray = (char**)malloc(10*sizeof(char*));
	char* page = _fetch_fn(start);
	assert (page != NULL);
	//printf("PAGE: %s\n", page);
	
	char* linkline = strstr(page, "link:");
	//first link
	*linkArray = linkline+(5*sizeof(char)); //linkline points to "l", so +5 char to get the link that comes after ":"? Not sure if this makes sense;
	//TODO: loop to get other links
	Queue_enqueue(link, linkqueue);
}