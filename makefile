CC=gcc
CFLAGS= -ansi -pedantic -Wall -Wextra -fsyntax-only

all: build1 libs1 build2 libs2

TARGET1=main

build1: libs1
	$(CC) $(TARGET1).c plot.c fileManagement.c graph.c simulation.c pbPlots.o supportLib.o -lm -o $(TARGET1).out && $(CC) simulation.c -o test $(CFLAGS)

libs1:
	$(CC) -c lib/pbPlots.c lib/supportLib.c -lm

TARGET2=autoTest

build2: libs2
	$(CC) $(TARGET2).c simulation.c supportLib.o -lm -o $(TARGET2).out && $(CC) simulation.c -o test $(CFLAGS)

libs2:
	$(CC) -c lib/supportLib.c -lm
