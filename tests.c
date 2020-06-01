#include "unit_test.h"
#include "defs.h"
#include "fft_convolve.h"

#include <stdio.h>
#include <fftw.h>

int playAudioStereo()
{
    struct sample_data data;
    int result;

    if (read_samples_from_wavfile("audio_files/piano2.wav", &data) == SUCCESS) {
        start_audio_systems();
        play_audio_samples(&data);
        terminate_audio_systems();
        free_sample_data(&data);
        result = PASS;
    } else {
        result = FAIL;
    }

    return result;
}

int writeSamplesToFile()
{
    struct sample_data data;

    read_samples_from_wavfile("audio_files/piano2.wav", &data);
    write_samples_to_wavfile("audio_files/out.wav", &data);
    free_sample_data(&data);

    return PASS;
}

int lowPassFilterWavefile()
{
    struct sample_data data;
    struct impulse_response_data lpf;
    int result;

    if (read_samples_from_wavfile("audio_files/piano2.wav", &data) == SUCCESS) {
        build_filter(&lpf, 0.0113f, 40.0f);
        apply_impulse_response(&data, &lpf);
        start_audio_systems();
        play_audio_samples(&data);
        terminate_audio_systems();
        free_sample_data(&data);
        result = PASS;
    } else {
        result = FAIL;
    }

    return result;
}

int fft()
{
    const int N = 65536;
    fftw_complex in[N];
    fftw_complex out[N];
    fftw_plan fwd;
    fftw_plan rvs;
    struct sample_data data;
    FILE *fp;

    // create fft and inverse fft plans
    fwd = fftw_create_plan(N, FFTW_FORWARD, FFTW_ESTIMATE);
    rvs = fftw_create_plan(N, FFTW_BACKWARD, FFTW_ESTIMATE);

    // get samples into the in array and print them
    fp = fopen("samples.dat", "w");
    read_samples_from_wavfile("audio_files/piano2.wav", &data);
    for (int i = 0; i < N; i++) {
        in[i].re = data.frames[i].left;
        in[i].im = 0;
    }
    print_real_part(fp, in, N);
    fclose(fp);

    // perform fourier transform and print results
    fftw_one(fwd, in, out);
    for (int i = 0; i < N; i++) {
        out[i].re /= 65536.0f;
        out[i].im /= 65536.0f;
    }
    fp = fopen("dft.dat", "w");
    print_complex_array_mag(fp, out, N);
    fclose(fp);

    // perform inverse transform and print results
    fftw_one(rvs, out, in);
    fp = fopen("inverse.dat", "w");
    print_real_part(fp, in, N);
    fclose(fp);

    // clean up
    fftw_destroy_plan(fwd);
    fftw_destroy_plan(rvs);
    free_sample_data(&data);

    return PASS;
}

int fftConvolution()
{
    Convolver c;

    init_convolver(&c, NULL, 0, 0);
    return PASS;
}

TESTS = {
    /*
    {"stereo audio test 1", playAudioStereo},
    {"wave file write test 1", writeSamplesToFile},
    {"low pass filter test", lowPassFilterWavefile},
    {"fft", fft},
    */
    {"convolution testing", fftConvolution},
    {0}
};
