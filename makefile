CC=gcc
CFLAGS=-ansi -pedantic -Wall -Wextra
TARGET=main

$(TARGET).out: $(TARGET).c
	$(CC) $(TARGET).c -o $(TARGET).out $(CFLAGS) -lm
