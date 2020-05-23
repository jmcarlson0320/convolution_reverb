CC = gcc
CFLAGS = -g -Wall
LIBS = -lm -lportaudio -lsndfile -lliquid
EXE = reverb

$(EXE): main.o audio.o utils.o
	$(CC) -o $@ $^ $(LIBS)

tests: tests.o
	$(CC) -o run_tests $^ $(LIBS)

tests.o: unit_test.h
audio.o: defs.h
utils.o: defs.h

clean:
	rm *.o $(EXE) run_tests

.PHONY: clean tests
