/*
 * The key to getting this going was an article i found about convolution
 * architecture on a random blog:
 * https://dvcs.w3.org/hg/audio/raw-file/tip/webaudio/convolution.html
 * This detailed what needed to be done when both the signal and the IR is
 * long. The method for convolving a long signal with a short IR turns out is
 * pretty standard, the overlap-add method. The clever chopping up of the IR
 * for long IRs is not so standard, this is the only place I could find any
 * info on it.
 *
 * This is my implementation of the Overlap-Add method
 * This convolver structure holds the saved state needed to perform the
 * overlap-add method. Successive calls to fft_convolve with the same
 * Convolver object automatically saves/adds the overlap. to get the final
 * tail that trails the original signal, call fft_convolve with a zero'd out
 * input. this adds the overlap to zero, resulting in just the overlap.
 * */
#ifndef FFT_CONVOLVE_H
#define FFT_CONVOLVE_H

#include <fftw.h>

typedef struct convolver {
    fftw_complex *ir_time;
    fftw_complex *ir_frequency;
    fftw_complex *input_buff;
    fftw_complex *output_buff;
    float *prev_overlap;
    int sample_block_size;
    int ir_size;
    float ir_scale;
    int dft_size;
    fftw_plan fft;
    fftw_plan ifft;
} Convolver;

void init_convolver(Convolver *conv, float *ir, int len, int segment_size);
void destroy_convolver(Convolver *conv);
void fft_convolve(Convolver *conv, float *input, float *output);

#endif // FFT_CONVOLVE_H
