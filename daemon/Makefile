
all: main

main: main.o daemon.o
	gcc main.o daemon.o -o mydaemon

main.o:
	gcc -c main.c

daemon.o:
	gcc -c daemon.c

