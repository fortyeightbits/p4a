.PHONY: all
all : libcrawler.so file_tester web_tester

file_tester : file_tester.c libcrawler.so
	gcc -g -L. -lcrawler -lpthread file_tester.c -Wall -Werror -o file_tester

web_tester : web_tester.c cs537.c libcrawler.so
	gcc -g -L. -lcrawler -lpthread web_tester.c cs537.c -Wall -Werror -o web_tester

libcrawler.so : crawler.c
	#gcc -g -shared -fPIC -o libcrawler.so crawler.c linkqueue.c pagequeue.c
	gcc -g -fPIC -c crawler.c -Wall -o crawler.o
	gcc -g -fPIC -c linkqueue.c -Wall -o linkqueue.o
	gcc -g -fPIC -c pagequeue.c -Wall -o pagequeue.o
	gcc -g -shared -o libcrawler.so crawler.o linkqueue.o pagequeue.o

.PHONY: clean
clean :
	rm -f file_tester web_tester libcrawler.so *.o *~


        #gcc -g -fPIC -c crawler.c -Wall -Werror -o crawler.o
        #gcc -g -shared -o libcrawler.so crawler.o

