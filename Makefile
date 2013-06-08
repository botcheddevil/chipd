CC ?= gcc
FORMATTER = ~/astyle/build/mac/bin/astyle

chipd: format hash.o http_parser.o chipd.o
	$(CC) hash.o http_parser.o chipd.o -o $@

format: 
	$(FORMATTER) --style=bsd --indent=spaces=4 chipd.c
	$(FORMATTER) --style=bsd --indent=spaces=4 hash.c

http_parser.o: http_parser.c http_parser.h
	$(CC) -c http_parser.c

hash.o: hash.c
	$(CC) -c hash.c

chip.o:
	$(CC) -pthread -c chip.c

clean:
	rm -f *.o a.out chipd chipd.c.orig
    
