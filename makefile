CC=gcc
CFILES=$(wildcard *.c)
CFLAGS=-O3
OUTPUT=zed-launcher.exe

main:
	$(CC) $(CFILES) -o $(OUTPUT) $(CFLAGS)

clean:
	rm .\$(OUTPUT)
