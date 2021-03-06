#---------------------
#Environment Variables
#---------------------

CC ?= gcc
FORMATTER = ~/Repos/astyle/build/mac/bin/astyle
DEBUG_FLAG = -g
#------------
#Final Binary
#------------

chipd: format hash.o http_parser.o server.o signals.o load.o chipd.o
	$(CC) $(DEBUG_FLAG) hash.o http_parser.o server.o signals.o load.o chipd.o -o $@ -lz

chipd.o: chipd.c
	$(CC) $(DEBUG_FLAG) -c chipd.c

#------------------------
#Compile External Library
#------------------------

hash.o: hash.c
	$(CC) $(DEBUG_FLAG) -c hash.c

load.o: load.c
	$(CC) $(DEBUG_FLAG) -c load.c

server.o: server.c
	$(CC) $(DEBUG_FLAG) -c server.c

signals.o: signals.c
	$(CC) $(DEBUG_FLAG) -c signals.c

#---------------
#Auto Formatting
#---------------

format:
	$(FORMATTER) --verbose --style=bsd --indent=spaces=4 \
		--align-pointer=name --pad-paren \
		--pad-header --add-brackets \
		--convert-tabs --break-after-logical\
		--max-code-length=80 --errors-to-stdout\
		chipd.c hash.c hash.h load.c load.h chipd.h tests/test_hash.c \
		tests/test_load.c

#------------------------
#Compile External Library
#------------------------

http_parser.o: http_parser.c http_parser.h
	$(CC) $(DEBUG_FLAG) -c http_parser.c

zlib:
	cd ./lib/zlib-1.2.8/;./configure;make

#------------------------
#Compile Tests
#------------------------

test_hash: hash.o test_hash.o
	$(CC) hash.o test_hash.o -o $@

test_hash.o:
	$(CC) -c ./tests/test_hash.c

test_load: format zlib load.o test_load.o
	$(CC) load.o test_load.o -o $@ -lz -L./lib/zlib-1.2.8/

test_load.o:
	$(CC) -c ./tests/test_load.c

clean:
	rm -f *.o a.out chipd test *.orig
	rm -f ./tests/*.orig
	rm -f configure.log
