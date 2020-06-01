#ifndef FFT_CONVOLVE_H
#define FFT_CONVOLVE_H

void init_convolver(float ir, int len, int segment_size);
void fft_convolve(float *input, float *output, int len);

#endif // FFT_CONVOLVE_H
