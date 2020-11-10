CC=gcc
CFLAGS= -Wall -Wextra
TARGET=main

build: libs
	$(CC) $(TARGET).c plot.c fileManagement.c graph.c simulation.c pbPlots.o supportLib.o -lm $(CFLAGS) -o $(TARGET)

libs:
	$(CC) -c lib/pbPlots.c lib/supportLib.c -lm 