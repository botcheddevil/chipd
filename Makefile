#---------------------
#Environment Variables
#---------------------

CC ?= gcc
FORMATTER = ~/Repos/astyle/build/mac/bin/astyle

#------------
#Final Binary
#------------

chipd: format hash.o http_parser.o chipd.o
	$(CC) hash.o http_parser.o chipd.o -o $@

chip.o:
	$(CC) -pthread -c chip.c

#------------------------
#Compile External Library
#------------------------

hash.o: hash.c
	$(CC) -c hash.c

load.o: load.c
	$(CC) -c load.c

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
	$(CC) -c http_parser.c

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
	rm -f ./lib/zlib-1.2.8/*.o ./lib/zlib-1.2.8/*.dylib
	rm -f configure.log
