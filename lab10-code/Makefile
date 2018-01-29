CFLAGS = -Wall -g
CC     = gcc $(CFLAGS)

all : philosophers_ipc philosophers_pthread

philosophers_ipc : philosophers_ipc.c
	$(CC) -o $@ $^

philosophers_pthread: philosophers_pthread.c
	$(CC) -pthread -o $@ $^

clean :
	rm -f *.o philosophers_ipc # philosophers_pthread



