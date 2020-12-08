CC=gcc
CFLAGS= -ansi -pedantic -Wall -Wextra -fsyntax-only

all: buildMain runTest libs

buildMain: libs
	$(CC) main.c plot.c fileManagement.c graph.c simulation.c distribution.c pbPlots.o supportLib.o -lm -lgsl -lgslcblas -o main.out && $(CC) simulation.c -o test $(CFLAGS)

runTest: libs
	$(CC) autoTest.c simulation.c distribution.c supportLib.o -lm -o autoTest.out && $(CC) simulation.c -o test $(CFLAGS)

libs:
	$(CC) -c lib/pbPlots.c lib/supportLib.c -lm
