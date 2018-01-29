CFLAGS = -Wall -g
CC = gcc $(CFLAGS)

clock_sim: lcd_clock.o lcd_clock_main.o lcd_clock.h lcd_update.o
	$(CC) -o clock_sim $^
	@echo "clock_sim ready for duty"

lcd_clock.o: lcd_clock.c lcd_clock.h
	$(CC) -c $<

lcd_clock_main.o: lcd_clock_main.c lcd_clock.h
	$(CC) -c $<

lcd_update.o: lcd_update.c lcd_clock.h
	$(CC) -c $<

test: clock_tests
	@echo "Running clock_tests"
	./clock_tests
	@echo "Tests Complete"

clock_tests: lcd_tests.c lcd_clock.o lcd_update.o lcd_clock.h
	$(CC) -o $@ $^

clean:
	rm -f *.o
