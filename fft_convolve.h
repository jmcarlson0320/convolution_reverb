#ifndef FFT_CONVOLVE_H
#define FFT_CONVOLVE_H

#include <fftw.h>

typedef struct convolver {
    float *ir_time;
    fftw_complex *ir_frequency;
    float *prev_overlap;
    int sample_block_size;
    int ir_size;
    int dft_size;
    fftw_complex *input_buff;
    fftw_complex *output_buff;
} Convolver;

void init_convolver(Convolver *conv, float *ir, int len, int segment_size);
void destroy_convolver(Convolver *conv);
void fft_convolve(Convolver *conv, float *input, float *output);

#endif // FFT_CONVOLVE_H
