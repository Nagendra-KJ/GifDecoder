CC=gcc
decoder: decoder.o gifdec.o
	$(CC) -g decoder.c gifdec.c -o decoder -lm
