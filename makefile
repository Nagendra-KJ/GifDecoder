CC=gcc
test: test.o gifdec.o
	$(CC) -o test test.o gifdec.o	
