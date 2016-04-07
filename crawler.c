#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <pthread.h>
#include "linkqueue.h"
#include "pagequeue.h"

int crawl(char *start_url,
	  int download_workers,
	  int parse_workers,
	  int queue_size,
	  char * (*_fetch_fn)(char *url),
	  void (*_edge_fn)(char *from, char *to)) {

	//Set up queues
	L_Queue_t linkQueue;
	LinkQueue_init(&linkQueue);
	P_Queue_t pageQueue;
	PageQueue_init(&pageQueue);

	//Set up threads
	pthread_t downloader_pool[download_workers];
	pthread_t parser_pool[parse_workers];
	
	//create downloaders
	int d;
	for(d = 0; d < download_workers; d++)
	{
		//what to pass in? change producerArgs
		pthread_create(&downloader_pool[d], NULL, downloader, ((void*)(&producerArgs[k])));			
	}
	
	int p;
	for(p = 0; p < parse_workers; p++)
	{
		//what to pass in? change consumerArgs
		pthread_create(&parser_pool[p], NULL, parser, ((void*)(&consumerArgs[l])));	
	}

  return -1;
}

//downloader is the consumer of links
void* downloader(void *arg) {
    pthread_mutex_lock(&linkQueueMutex);
    while (linkQueue->size == 0) //sleep if there's no links to get
        Pthread_cond_wait(&linkQueueFill, &linkQueueMutex);
    downloadPage(); //TODO download function
    pthread_cond_signal(&linkQueueEmpty);
    pthread_mutex_unlock(&linkQueueMutex);
}

//parser is the producer of links
void* parser(void *arg) {
    pthread_mutex_lock(&linkQueueMutex);
    while (linkQueue->size == queue_size) //sleep if queue is full. Is this right?
        Pthread_cond_wait(&linkQueueEmpty, &linkQueueMutex);
    parsePage(); //TODO: parse page
    pthread_cond_signal(&linkQueueFill);
    pthread_mutex_unlock(&linkQueueMutex);
}

//BUGGLE!!! Not sure if I'm using callbacks/functionptr(?) right. :( Trying to pass _fetch_fn to this parsepage function.
void downloadPage(Queue_t* linkqueue, char *start, char * (*_fetch_fn)(char *url), Queue_t* pagequeue){

  char* dequeuedLink;
  char* content;
  Queue_dequeue(dequeuedLink, linkqueue);
  char* page = _fetch_fn(start);
    assert (page != NULL);
    //printf("PAGE: %s\n", page);

    //enqueue page content
    Queue_enqueue(page, pagequeue);
}

//dequeue from page queue, get links
void parsePage(Queue_t* pagequeue, void (*_edge_fn)(char *from, char *to), Queue_t* linkqueue){
  char* lineSavePtr = NULL;
  char* spaceSavePtr = NULL;
  char* dequeuedPage;
  char* line;
  char* link;
  int j;
  int i;
  char* delimiter1 = "\n";
  char* delimiter2 = " ";

  Queue_dequeue(dequeuedPage, pagequeue);
  //link:wisc.edu

  for (j = 0; ; j++, dequeuedPage = NULL) {
    line = strtok_r(dequeuedPage, delimiter1, &lineSavePtr);
    if (line == NULL)
        break;
    printf("Line %d: %s\n", j, line);

      for (i = 0; ; line = NULL, i++) {
        link = strtok_r(line, delimiter2, &spaceSavePtr);
        if (link == NULL)
            break;
        char* linkchunk = strstr(link, "link:");

        if(linkchunk != NULL)
        {
            linkchunk = linkchunk+(5*sizeof(char));
            removeLine(linkchunk);
            printf("here's the link: %s\n", linkchunk);
        }
         //TODO: how to get link of page it's currently on?
        //edge(currentlink, linkchunk);
        //Queue_enqueue(linkchunk, linkQueue);
        printf(" --> %s\n", link);
      }
  }
}

//removes \n
void removeLine (char* string){
    int length;
    if ((length = strlen(string)) >0){
        if (string[length - 1] == '\n')
            string[length - 1] = '\0';
    }
}
