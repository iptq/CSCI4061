CFLAGS = -Wall -g
CC     = gcc $(CFLAGS)

all : text2bin bin2text specific_stock

text2bin : text2bin.o
	$(CC) -o $@ $^

text2bin.o : text2bin.c header.h
	$(CC) -c $<

bin2text : bin2text.o 
	$(CC) -o $@ $^

bin2text.o : bin2text.c header.h
	$(CC) -c $<

specific_stock : specific_stock.o 
	$(CC) -o $@ $^

specific_stock.o : specific_stock.c header.h
	$(CC) -c $<


clean :
	rm -f *.o actual.txt expect.txt valgrind.txt

realclean : clean
	rm -f text2bin bin2text query_test
