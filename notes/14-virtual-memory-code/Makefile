CFLAGS = -Wall -g
CC     = gcc $(CFLAGS)
# LIBS   = -lpthread

all : mmap-specific-stock mmap-tr mmap-print-file mmap-demo page-size

page-size : page-size.c
	$(CC) -o $@ $^

mmap-demo : mmap-demo.c
	$(CC) -o $@ $^

mmap-print-file : mmap-print-file.c
	$(CC) -o $@ $^

mmap-tr : mmap-tr.c
	$(CC) -o $@ $^

mmap-specific-stock : mmap-specific-stock.c
	$(CC) -o $@ $^

clean :
	rm -f *.o mmap-specific-stock mmap-tr mmap-print-file mmap-demo page-size
