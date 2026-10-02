CC=gcc
WINDRES=windres
CFILES=$(wildcard *.c)
CFLAGS=-O3 -mwindows -lwinmm -lurlmon -lshell32 -lcomctl32 -luuid
OUTPUT=zed-launcher.exe

.PHONY: all clean

all: resource.o
	$(CC) $(CFILES) resource.o -o $(OUTPUT) $(CFLAGS)

resource.o: resource.rc zed.ico
	$(WINDRES) resource.rc -o resource.o

clean:
	rm -f $(OUTPUT) resource.o