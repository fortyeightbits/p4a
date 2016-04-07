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
		pthread_create(&downloader_pool[d], NULL, downloadHelper, ((void*)(&producerArgs[k])));			
	}
	
	int p;
	for(p = 0; p < parse_workers; p++)
	{
		//what to pass in? change consumerArgs
		pthread_create(&parser_pool[p], NULL, parseHelper, ((void*)(&consumerArgs[l])));	
	}

  return -1;
}

//downloader is the consumer of links
void* downloadHelper(void *arg) {

}

//parser is the producer of links
void* parseHelper(void *arg) {

}

char* downloadPage(char *link, char * (*_fetch_fn)(char *url)){

  char* page = _fetch_fn(link);
  assert (page != NULL);
  return page;
}

void parsePage(char* page){
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
