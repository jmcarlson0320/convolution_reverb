CC = gcc
CFLAGS = -g -Wall
LIBS = -lm -lportaudio -lsndfile -lliquid -lfftw
EXE = reverb

$(EXE): main.o audio.o utils.o reverberator.o fft_convolve.o mtap_buff.o
	$(CC) -o $@ $^ $(LIBS)

tests: tests.o audio.o utils.o fft_convolve.o mtap_buff.o
	$(CC) -o $@ $^ $(LIBS)

tests.o: unit_test.h defs.h mtap_buff.h
audio.o: defs.h
utils.o: defs.h
fft_convolve.o: fft_convolve.h defs.h
mtap_buff.o: mtap_buff.h
reverberator.o: defs.h

clean:
	rm *.o $(EXE) tests

.PHONY: clean
