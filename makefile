CC=gcc
CFILES=$(wildcard *.c)
CFLAGS=-O3 -mwindows -lwinmm
OUTPUT=zed-launcher.exe

.PHONY: all clean

all: main.c
	$(CC) $(CFILES) -o $(OUTPUT) $(CFLAGS)

clean:
	rm .\$(OUTPUT)
