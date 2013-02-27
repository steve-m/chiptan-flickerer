CC = gcc
CFLAGS = -g -Wall

program : chiptan.c
	$(CC) $(CFLAGS) -o chiptan chiptan.c
clean:
	rm -f chiptan

