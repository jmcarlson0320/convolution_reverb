CC = gcc
CFLAGS = -g -Wall
LIBS = -lm -lportaudio
EXE = reverb

$(EXE): main.o
	$(CC) -o $@ $^ $(LIBS)

tests: tests.o
	$(CC) -o run_tests $^ $(LIBS)

tests.o: unit_test.h

clean:
	rm *.o $(EXE) run_tests

.PHONY: clean tests
