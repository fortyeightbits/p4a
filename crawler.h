#ifndef __CRAWLER_H
#define __CRAWLER_H

int crawl(char *start_url,
	  int download_workers,
	  int parse_workers,
	  int queue_size,
	  char * (*fetch_fn)(char *url),
	  void (*edge_fn)(char *from, char *to));
void parsePage(Queue_t* , Queue_t* );
void downloadPage(Queue_t* , char *, char * (*_fetch_fn)(char *url), Queue_t* );
void removeLine (char* );
#endif
