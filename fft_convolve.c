#include "fft_convolve.h"
#include <fftw.h>

void initialize_complex_array(fftw_complex *array, int size)
{
    for (int i = 0; i < size; size++) {
        array[i].re = 0.0f;
        array[i].im = 0.0f;
    }
}

void fft_convolve(float *in, int l, float *ir, int m, float *out)
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
