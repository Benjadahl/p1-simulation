CC=gcc
CFLAGS= -ansi -pedantic -Wall -Wextra -fsyntax-only -lm
TARGET=main

build: libs
	$(CC) $(TARGET).c plot.c fileManagement.c graph.c simulation.c pbPlots.o supportLib.o -lm -o $(TARGET).out && $(CC) simulation.c -o test $(CFLAGS)

libs:
	$(CC) -c lib/pbPlots.c lib/supportLib.c -lm