CC=gcc
CFLAGS=-ansi -Wall -Wextra
TARGET=main

$(TARGET).out: $(TARGET).c
	$(CC) $(TARGET).c plot.o pbPlots.o supportLib.o -o $(TARGET).out $(CFLAGS) -lm

plot.o: plot.c lib/pbPlots.c lib/pbPlots.h lib/supportLib.c lib/supportLib.h
	$(CC) -c plot.c lib/pbPlots.c lib/supportLib.c -lm 

pbPlots.o: lib/pbPlots.c lib/pbPlots.h
	$(CC) -c lib/pbPlots.c -lm

supportLib.o: lib/supportLib.c lib/supportLib.h
	$(CC) -c lib/supportLib.c -lm
