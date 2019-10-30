CC=gcc
decoder: decoder.o gifdec.o
	$(CC) -o decoder decoder.o gifdec.o -lm
