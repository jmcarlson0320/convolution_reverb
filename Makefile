CC = gcc
CFLAGS = -g -Wall
LIBS = -lm -lportaudio -lsndfile -lliquid -lfftw
EXE = reverb

$(EXE): main.o audio.o utils.o
	$(CC) -o $@ $^ $(LIBS)

tests: tests.o audio.o utils.o fft_convolve.o
	$(CC) -o run_tests $^ $(LIBS)

tests.o: unit_test.h defs.h
audio.o: defs.h
utils.o: defs.h
fft_convolve.o: fft_convolve.h

clean:
	rm *.o $(EXE) run_tests

.PHONY: clean tests
