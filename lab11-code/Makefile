# Makefile build/run experiments using odds_evens_* programs and
# compare timings of variants.

# print only when lock acquired
CC = gcc -Wall -DTHREAD_ITERS=10000 -DVPRINTF=ignore 
LIBS = -lpthread

# # don't sleep at all
# CC = gcc -Wall -Wno-unused-value -lpthread -DTHREAD_ITERS=5000 -DVPRINTF=ignore -DSLEEPFUN=0 

# also print when failing to acquire
# CC = gcc -Wall -lpthread -DTHREAD_ITERS=5000

PROGS = busy nested_if condvar 

all : $(PROGS)

experiment : all
	for prog in $(PROGS); do echo "time $$prog:"; time ./$$prog > /tmp/$$prog.out; echo "-----"; done
	for prog in $(PROGS); do wc -l /tmp/$$prog.out; done

busy : odds_evens_busy.c odds_evens.h
	$(CC) -o $@ $< $(LIBS)

nested_if : odds_evens_busy_nested_if.c odds_evens.h
	$(CC) -o $@ $< $(LIBS)

condvar : odds_evens_condvar.c odds_evens.h
	$(CC) -o $@ $< $(LIBS)



clean :
	rm -f $(PROGS)

