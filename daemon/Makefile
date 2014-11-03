PROG_NAME = mydaemon

srcfiles = $(wildcard *.c)
objects = $(srcfiles:%.c=%.o)

CFLAGS = -O2
CC = gcc

.SUFFIXES: .c .o

.c.o:
	gcc -c $< -o $@

mydaemon: $(objects)
	$(CC) $(CFLAGS) $(objects) -o $(PROG_NAME)

.PHONY: clean
clean:
	-rm $(objects)
