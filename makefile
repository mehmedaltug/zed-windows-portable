CC=gcc
CFILES=$(wildcard *.c)
OUTPUT=zed-launcher.exe

main:
	$(CC) $(CFILES) -o $(OUTPUT)

clean:
	rm .\$(OUTPUT)
