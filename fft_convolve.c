#include "fft_convolve.h"

#include <stdlib.h>
#include <math.h>

#include <fftw.h>

#include "defs.h"

// TODO
// clear buffers
void init_convolver(Convolver *conv, float *ir, int len, int segment_size)
{
    conv->ir_size = len;
    conv->sample_block_size = segment_size;
    conv->dft_size = conv->sample_block_size + conv->ir_size - 1;

    conv->ir_time = malloc(sizeof(fftw_complex) * conv->dft_size);
    conv->ir_frequency = malloc(sizeof(fftw_complex) * conv->dft_size);
    conv->prev_overlap = malloc(sizeof(float) * conv->ir_size - 1);
    conv->input_buff = malloc(sizeof(fftw_complex) * conv->dft_size);
    conv->output_buff = malloc(sizeof(fftw_complex) * conv->dft_size);

    conv->fft = fftw_create_plan(conv->dft_size, FFTW_FORWARD, FFTW_ESTIMATE);
    conv->ifft = fftw_create_plan(conv->dft_size, FFTW_BACKWARD, FFTW_ESTIMATE);

    // copy ir to ir_buffer and pad w/ zeros
    for (int i = 0; i < conv->dft_size; i++) {
        if (i < conv->ir_size) {
            conv->ir_time[i].re = ir[i];
            conv->ir_time[i].im = 0.0f;
        } else {
            conv->ir_time[i].re = 0.0f;
            conv->ir_time[i].im = 0.0f;
        }
    }

    // clear the overlap buffer
    for (int i = 0; i < conv->ir_size - 1; i++) {
        conv->prev_overlap[i] = 0.0f;
    }

    // generate frequency data of impulse response
    fftw_one(conv->fft, conv->ir_time, conv->ir_frequency);

    // calculate a scaling factor, not sure if this is legit
    conv->ir_scale = 0.0f;
    for (int i = 0; i < conv->dft_size; i++) {
        conv->ir_scale += conv->ir_time[i].re;
    }
}

void destroy_convolver(Convolver *conv)
{
    free(conv->ir_time);
    free(conv->ir_frequency);
    free(conv->prev_overlap);
    free(conv->input_buff);
    free(conv->output_buff);
    conv->ir_time = NULL;
    conv->ir_frequency = NULL;
    conv->prev_overlap = NULL;
    conv->input_buff = NULL;
    conv->output_buff = NULL;
    fftw_destroy_plan(conv->fft);
    fftw_destroy_plan(conv->ifft);
}

// TODO
// calculate 1/N inside init function
void fft_convolve(Convolver *conv, float *input, float *output)
{
    /*
     * Algorithm (add-overlap method)
     *  1. perform fft on input, storing data in input_buff
     *     (pad w/ zeros)
     *  2. complex multiply with ir_frequency, storing data in
     *     output_buff
     *  3. perform ifft on output_buff (in-place if possible)
     *  4. split output_buff in two: the first "sample_block_size"
     *     number of samples, and the remaining "ir_size - 1" samples
     *     (the overlap)
     *  5. add prev_overlap to the output sample block and write to
     *     out array
     *  6. copy the overlap to prev_overlap
     *
     * */

    // step 1
    // init a temporary array
    fftw_complex tmp[conv->dft_size];
    for (int i = 0; i < conv->dft_size; i++) {
        tmp[i].re = 0.0f;
        tmp[i].im = 0.0f;
    }

    // copy input and pad
    for (int i = 0; i < conv->dft_size; i++) {
        if (i < conv->sample_block_size) {
            conv->input_buff[i].re = input[i];
            conv->input_buff[i].im = 0.0f;
        } else {
            conv->input_buff[i].re = 0.0f;
            conv->input_buff[i].im = 0.0f;
        }
    }

    // transform input to frequency domain
    fftw_one(conv->fft, conv->input_buff, tmp);

    // step 2
    // perform convolution by multiplication in the frequency domain
    for (int i = 0; i < conv->dft_size; i++) {
        tmp[i] = complex_multiply(tmp[i], conv->ir_frequency[i]);
    }

    // step 3
    // inverse transform back to time domain
    fftw_one(conv->ifft, tmp, conv->output_buff);

    // step 4, 5, 6
    for (int i = 0; i < conv->dft_size; i++) {
        // combine output with existing overlap
        if (i < conv->ir_size - 1)
            output[i] = conv->output_buff[i].re / (float) conv->dft_size + conv->prev_overlap[i];
        // copy rest of output
        else if (i < conv->sample_block_size)
            output[i] = conv->output_buff[i].re / (float) conv->dft_size;
        // save new overlap for next time
        else // TODO this is WRONG!!!!!
            conv->prev_overlap[i - conv->sample_block_size] = conv->output_buff[i].re / (float) conv->dft_size;
    }
}
