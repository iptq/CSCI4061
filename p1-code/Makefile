CFLAGS = -Wall -g
CC = gcc

commando: commando.o cmd.o cmdctl.o util.o
	$(CC) $(CFLAGS) -o commando $^

commando.o: commando.c commando.h
	$(CC) $(CFLAGS) -c $<

cmd.o: cmd.c commando.h
	$(CC) $(CFLAGS) -c $<

cmdctl.o: cmdctl.c commando.h
	$(CC) $(CFLAGS) -c $<

util.o: util.c commando.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o actual.txt expect.txt valgrind.txt

binary_tests.o : binary_tests.c commando.h tests.h 
	$(CC) -c $<

test_utils.o : test_utils.c tests.h
	$(CC) -c $<

binary_tests : binary_tests.o cmd.o cmdctl.o test_utils.o
	$(CC) -o $@ $^

test-binary : binary_tests
	valgrind ./binary_tests 

test-shell : commando
	./shell_tests.sh
