CC=gcc
CFLAGS=-g
srclib=-lrt -pthread -lconfuse
srclib2 = -lpicohttpparser -lhttp

PROGS =	server #client
OBJS = obj/utils.o obj/http.o obj/server.o obj/picohttpparser.o 
LIB = lib/libpicohttpparser.a lib/libhttp.a

all: objects server

server: $(LIB) obj/server.o obj/utils.o obj/http.o obj/picohttpparser.o
	$(CC) $(CFLAGS) -o $@ $^ $(srclib) $(srclib2) -Llib/

objects:
	mkdir lib
	mkdir obj

obj/utils.o: src/utils.c includes/utils.h
	$(CC) $(CFLAGS) -c -o $@ $<

obj/http.o: src/http.c includes/http.h srclib/picohttpparser.h
	$(CC) $(CFLAGS) -c -o $@ $<

obj/server.o: src/server.c includes/utils.h includes/http.h
	$(CC) $(CFLAGS) -c -o $@ $<

obj/picohttpparser.o: srclib/picohttpparser.c srclib/picohttpparser.h
	$(CC) $(CFLAGS) -c -o $@ $<

# libraries
lib/libpicohttpparser.a: obj/picohttpparser.o
	ar -rv $@ $^

lib/libhttp.a: obj/http.o
	ar -rv $@ $^

clean:
		rm -f ${PROGS} $(OBJS)
		rm -R lib
		rm -R obj
