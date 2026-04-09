CC=gcc
CFILES=$(wildcard *.c)
CFLAGS=-O3 -mwindows -lwinmm
OUTPUT=zed-launcher.exe

main:
	$(CC) $(CFILES) -o $(OUTPUT) $(CFLAGS)

clean:
	rm .\$(OUTPUT)
