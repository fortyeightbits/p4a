#ifndef __CRAWLER_H
#define __CRAWLER_H
// typdefines
#include "linkqueue.h"
#include "pagequeue.h"

typedef struct parseArgPacket{
    L_Queue_t* linkqueue;
    P_Queue_t* pagequeue;
    int queue_size;
    void (*_edge_fn)(char *from, char *to);
} pArgs_t;

typedef struct downloadArgPacket{
    L_Queue_t* linkqueue;
    P_Queue_t* pagequeue;
    char * (*_fetch_fn)(char *url);
} dArgs_t;


// Function prototypes
int crawl(char *start_url,
	  int download_workers,
	  int parse_workers,
	  int queue_size,
	  char * (*fetch_fn)(char *url),
	  void (*edge_fn)(char *from, char *to));
void removeLine (char* );
void* downloadHelper(void *);
void* parseHelper(void *);
void parsePage(char* , char* , void (*_edge_fn)(char *from, char *to), L_Queue_t* , int );
int workInSystem;


#endif
