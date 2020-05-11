CC = gcc
CFLAGS = -pedantic -Wall -D_POSIX_C_SOURCE=200112L
LDLIBS = -lpthread -lrt

.PHONY: all clean

main: main.o utilities.o

main.o: main.c

utilities.o: utilities.c

clean:	
	-rm -f main *.o

