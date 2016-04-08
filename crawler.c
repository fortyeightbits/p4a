#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <pthread.h>
#include "linkqueue.h"
#include "pagequeue.h"
#include "crawler.h"

int crawl(char *start_url,
	  int download_workers,
	  int parse_workers,
	  int queue_size,
	  char * (*_fetch_fn)(char *url),
	  void (*_edge_fn)(char *from, char *to)) { 
	//Set up queues
	L_Queue_t linkQueue;
	LinkQueue_init(&linkQueue);
    LinkQueue_enqueue(start_url,linkQueue,queue_size);

	P_Queue_t pageQueue;
	PageQueue_init(&pageQueue);

    // Instantiate downloadHelper nad parseHelper void* args
    pArgs_t parserArgs;
    parserArgs.queue_size = queue_size;
    parserArgs.linkqueue = &linkQueue;
    parserArgs.pagequeue = &pageQueue;
    parserArgs._edge_fn = _edge_fn;

    dArgs_t downloadArgs;
    downloadArgs.linkqueue = &linkQueue;
    downloadArgs.pagequeue = &pageQueue;
    downloadArgs._fetch_fn = _fetch_fn;

	//Set up threads
	pthread_t downloader_pool[download_workers];
	pthread_t parser_pool[parse_workers];
	
	//create downloaders
	int d;
	for(d = 0; d < download_workers; d++)
	{
        // Create download workers here. Run along now my little minions!
        pthread_create(&downloader_pool[d], NULL, downloadHelper, ((void*)(&downloadArgs)));
	}
	
	int p;
	for(p = 0; p < parse_workers; p++)
	{
        // Create parse workers here. Run along too my little parsers!
        pthread_create(&parser_pool[p], NULL, parseHelper, ((void*)(&parserArgs)));
	}

    // TODO: Join here. Main sleeps until all workers have completed running work in system.
  return -1;
}

//downloader is the consumer of links
void* downloadHelper(void *arg) {   
    // Unpacking and creating local references to queues and fetch function
    char * (*_fetch_fn)(char *url) = ((dArgs_t*)arg)->_fetch_fn;
    L_Queue_t linkQueue = ((dArgs_t*)arg)->linkqueue;
    P_Queue_t pageQueue = ((dArgs_t*)arg)->pagequeue;
    // ---------------------------------------------------------------------
    char* returnValue = (char*)malloc(100*sizeof(char));
    char* page = (char*)malloc(1000*sizeof(char));

    while (1)
    {
        LinkQueue_dequeue(linkQueue, &returnValue);
        strcpy(page,*_fetch_fn(returnValue));
        PageQueue_enqueue(page, &pageQueue);
    }
}

//parser is the producer of links
void* parseHelper(void *arg) {
    // Unpacking and creating local references to queues and edge function
    void (*_edge_fn)(char *from, char *to) = ((pArgs_t*)arg)->_edge_fn;
    L_Queue_t linkQueue = ((pArgs_t*)arg)->linkqueue;
    P_Queue_t pageQueue = ((pArgs_t*)arg)->pagequeue;
    int linkQueue_size = ((pArgs_t*)arg)->queue_size;
    // ---------------------------------------------------------------------
    char* returnValue = (char*)malloc(1000*sizeof(char));
    char* link = (char*)malloc(100*sizeof(char));
    while (1)
    {
        PageQueue_dequeue(&pageQueue, &returnValue, &link);
        parsePage(returnValue, link, _edge_fn, &linkQueue, linkQueue_size);
    }
}

char* downloadPage(char *link, char * (*_fetch_fn)(char *url)){

  char* page = _fetch_fn(link);
  assert (page != NULL);
  return page;
}

void parsePage(char* page, char* pagename, void (*_edge_fn)(char *from, char *to), L_Queue_t* linkQueue, int linkQueue_size){
  char* lineSavePtr = NULL;
  char* spaceSavePtr = NULL;
  char* line;
  char* link;
  int j;
  int i;
  char* delimiter1 = "\n";
  char* delimiter2 = " ";


  for (j = 0; ; j++, page = NULL) {
    line = strtok_r(page, delimiter1, &lineSavePtr);
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
            //TODO: Check if linkchunk is in hash table
            *_edge_fn(pagename, linkchunk);
            LinkQueue_enqueue(linkchunk, linkQueue, linkQueue_size);
        }
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
