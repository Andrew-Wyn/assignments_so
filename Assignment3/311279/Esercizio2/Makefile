CC = gcc
CFLAGS = -Wall -pedantic 
LDLIBS = -lpthread -lrt

TARGETS = client server

.PHONY: all clean

all : $(TARGETS)

client: client.o util.o

server: server.o util.o

client.o: client.c

server.o: server.c

util.o: util.c

clean : 
	-rm $(TARGETS)
	-rm ./*.o
