CC=gcc
CFLAGS= -ansi -pedantic -Wall -Wextra -fsyntax-only
CFLAGS1= -lm -lgsl -lgslcblas -g

all: buildMain runTest libs

buildMain: libs
	$(CC) main.c plot.c fileManagement.c graph.c simulation.c pbPlots.o supportLib.o $(CFLAGS1) -o main.out && $(CC) simulation.c -o test $(CFLAGS)

runTest: libs
	$(CC) autoTest.c simulation.c supportLib.o $(CFLAGS1) -o autoTest.out && $(CC) simulation.c -o test $(CFLAGS)

libs:
	$(CC) -c lib/pbPlots.c lib/supportLib.c -lm
