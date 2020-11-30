CC=gcc
CFLAGS= -ansi -pedantic -Wall -Wextra -fsyntax-only

all: buildMain runTest libs

buildMain: libs
	$(CC) main.c plot.c fileManagement.c graph.c simulation.c pbPlots.o supportLib.o -lm -o main.out && $(CC) simulation.c -o test $(CFLAGS)

runTest: libs
	$(CC) autoTest.c simulation.c supportLib.o -lm -o autoTest.out && $(CC) simulation.c -o test $(CFLAGS)

libs:
	$(CC) -c lib/pbPlots.c lib/supportLib.c -lm