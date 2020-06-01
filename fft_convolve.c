#include "fft_convolve.h"
#include <fftw.h>

// TODO
// allocate all the buffers
// generate the ir_frequency data
void init_convolver(Convolver *conv, float *ir, int len, int segment_size)
{
    conv->ir_time = NULL;
    conv->ir_frequency = NULL;
    conv->ir_size = len;
    conv->prev_overlap = NULL;
    conv->sample_block_size = segment_size;
    conv->dft_size = conv->sample_block_size + conv->ir_size - 1;
    conv->input_buff = NULL;
    conv->output_buff = NULL;
}

// TODO
// free all the buffers, and set to NULL
void destroy_convolver(Convolver *conv)
{
}

// TODO
// implement
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
}

/*
void fft_convolve(float *in, float *out, int len)
{
    fftw_complex input_samples[l + m - 1];
    fftw_complex input_spectrum[l + m - 1];
    fftw_complex impulse_response[l + m - 1];
    fftw_complex impulse_response_spectrum[l + m - 1];
    fftw_complex output_spectrum[l + m - 1];
    fftw_complex output_samples[l + m - 1];

    fftw_plan fft = fftw_create_plan(l + m - 1, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_plan ifft = fftw_create_plan(l + m - 1, FFTW_BACKWARD, FFTW_ESTIMATE);

    // load samples for processing and pad w/ zeros
    for (int i = 0; i < l + m - 1; i++) {
        input_samples[i].im = 0.0f;
        if (i < l)
            input_samples[i].re = in[i];
        else
            input_samples[i].re = 0.0f;
    }

    // convert samples to freq domain
    fftw_one(fft, input_samples, input_spectrum);

    // load impulse response and pad w/ zeros
    for (int i = 0; i < l + m - 1; i++) {
        impulse_response[i].im = 0.0f;
        if (i < m)
            impulse_response[i].re = ir[i];
        else {
            impulse_response[i].re = 0.0f;
        }
    }

    // convert ir to freq domain
    fftw_one(fft, impulse_response, impulse_response_spectrum);

    // perform complex multiplication
    for (int i = 0; i < l + m - 1; i++) {
        output_spectrum[i].re = input_spectrum[i].re * impulse_response_spectrum[i].re
                              - input_spectrum[i].im * impulse_response_spectrum[i].im;
        output_spectrum[i].im = input_spectrum[i].re * impulse_response_spectrum[i].im
                              + input_spectrum[i].im * impulse_response_spectrum[i].re;
    }

    // convert back to time domain
    fftw_one(ifft, output_spectrum, output_samples);

    // write real part to output
    float scale_factor = 1.0f / (float) (l + m - 1);
    for (int i = 0; i < l + m - 1; i++) {
        out[i] = output_samples[i].re  * scale_factor;
    }
}
*/
