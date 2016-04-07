#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <pthread.h>

void removeLine (char* string){
    int length;
    if ((length = strlen(string)) >0){
        if (string[length - 1] == '\n')
            string[length - 1] = '\0';
    }
  }

int main(int argc, char* argv[])
{

	/*char* dequeuedPage = (char* )malloc(130*sizeof(char));
		strcpy(dequeuedPage, "link:meow\n");
	*/
	char* dequeuedPage = "blah\n";
	
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
		  linkchunk = linkcunk+(5*sizeof(char));
		  removeLine(linkchunk);
		
		   //TODO: how to get link of page it's currently on?
		  //edge(currentlink, linkchunk);
		  //Queue_enqueue(linkchunk, linkQueue);
		 
          printf(" --> %s\n", link);
		  printf("here's the link: %s\n", linkchunk));
      }
  }
  
	char* linkchunk = strstr(dequeuedPage, "link:");
	removeLine(linkchunk);
	printf("here's the link: %s\n", linkchunk+(5*sizeof(char)));
		
		
}
