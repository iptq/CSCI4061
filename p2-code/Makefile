CC = gcc
CFLAGS = -g -Wall -Werror

.PHONY: all handin clean clean-tests

all: bindir bl-server bl-client bl-showlog

bindir:
	mkdir -p bin

bl-server: server.o util.o bl-server.o
	$(CC) $(CFLAGS) -o bin/bl-server $^

bl-server.o: bl-server.c
	$(CC) $(CFLAGS) -c -o $@ $<

server.o: server.c
	$(CC) $(CFLAGS) -c -o $@ $<

bl-client: util.o simpio.o bl-client.o
	$(CC) $(CFLAGS) -o bin/bl-client $^ -lpthread

bl-client.o: bl-client.c
	$(CC) $(CFLAGS) -c -o $@ $<

bl-showlog: bl-showlog.o
	$(CC) $(CFLAGS) -o bin/bl-showlog $<

bl-showlog.o: bl-showlog.c
	$(CC) $(CFLAGS) -c -o $@ $<

simpio.o: simpio.c
	$(CC) $(CFLAGS) -c -o $@ $<

util.o: util.c
	$(CC) $(CFLAGS) -c -o $@ $<

handin:
	zip handin GROUP-MEMBERS.txt Makefile bl-client.c bl-server.c bl-showlog.c blather.h server.c simpio.c util.c

clean:
	rm -f bin/{bl-server,bl-client} *.o *.fifo

shell-tests: shell_tests.sh shell_tests_data.sh clean-tests
	chmod u+rx shell_tests.sh shell_tests_data.sh normalize.awk filter-semopen-bug.awk
	./shell_tests.sh

clean-tests:
	rm -f test-*.{log,out,expect,diff,valgrindout}
	rm -f *.fifo

