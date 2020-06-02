#include "unit_test.h"
#include "defs.h"
#include "fft_convolve.h"
#include "mtap_buff.h"

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
    struct sample_data data;
    struct impulse_response_data lpf;
    float *samples;
    float *output;
    float *empty;

    // setup
    samples = malloc(85 * sizeof(float));
    output = malloc(85 * sizeof(float));
    empty = malloc(85 * sizeof(float));

    for (int i = 0; i < 85; i++) {
        empty[i] = 0.0f;
    }

    read_samples_from_wavfile("audio_files/piano2.wav", &data);

    // copy and print sample data
    for (int i = 0; i < 85; i++) {
        samples[i] = data.frames[i].left;
        output[i] = 0.0f;
    }
    FILE *fp = fopen("sample_data.dat", "w");
    print_float_array(fp, samples, 85);
    fclose(fp);
    fp = NULL;

    // build filter and print data
    build_filter(&lpf, 0.0113f, 40.0f);
    fp = fopen("filter_data.dat", "w");
    print_float_array(fp, lpf.data, lpf.num_points);
    fclose(fp);
    fp = NULL;

    // convolve
    init_convolver(&c, lpf.data, lpf.num_points, 128 - lpf.num_points + 1);
    fft_convolve(&c, samples, output);

    // print output data
    fp = fopen("convolved_samples.dat", "w");
    print_float_array(fp, output, 85);
    fclose(fp);
    fp = NULL;

    // clean up
    destroy_convolver(&c);

    free(lpf.data);

    free(empty);
    free(samples);
    free(output);

    free_sample_data(&data);

    return PASS;
}

int directConvolution()
{
    struct sample_data data;
    struct impulse_response_data lpf;

    read_samples_from_wavfile("audio_files/piano2.wav", &data);

    // build filter and print data
    build_filter(&lpf, 0.0113f, 40.0f);

    // convolve
    apply_impulse_response(&data, &lpf);

    float output[85];
    for (int i = 0; i < 85; i++) {
        output[i] = data.frames[i].left;
    }

    // print output data
    FILE *fp = fopen("convolved_samples.dat", "w");
    print_float_array(fp, output, 85);
    fclose(fp);
    fp = NULL;

    // clean up
    free(lpf.data);
    free_sample_data(&data);

    return PASS;
}

int segmentedConvolution()
{
    Convolver c_left;
    Convolver c_right;
    struct sample_data data;
    struct impulse_response_data lpf;
    int num_segments;
    float in_left[85];
    float in_right[85];
    float out_left[85];
    float out_right[85];

    read_samples_from_wavfile("audio_files/piano2.wav", &data);

    start_audio_systems();
    play_audio_samples(&data);
    terminate_audio_systems();

    build_filter(&lpf, 0.0113f, 40.0f);
    init_convolver(&c_left, lpf.data, lpf.num_points, 128 - lpf.num_points + 1);
    init_convolver(&c_right, lpf.data, lpf.num_points, 128 - lpf.num_points + 1);

    num_segments = data.num_frames / 85;

    // loop through all segments
    for (int cur_seg = 0; cur_seg < num_segments; cur_seg++) {
        // get next chunk of samples
        for (int i = 0; i < 85; i++) {
            in_left[i] = data.frames[i + cur_seg * 85].left;
            in_right[i] = data.frames[i + cur_seg * 85].right;
        }

        // perform convolution
        fft_convolve(&c_left, in_left, out_left);
        fft_convolve(&c_right, in_right, out_right);

        // write samples back
        for (int i = 0; i < 85; i++) {
            data.frames[i + cur_seg * 85].left = out_left[i];
            data.frames[i + cur_seg * 85].right = out_right[i];
        }
    }

    // play the filtered samples
    start_audio_systems();
    play_audio_samples(&data);
    terminate_audio_systems();

    // clean up
    destroy_convolver(&c_left);
    destroy_convolver(&c_right);
    free_sample_data(&data);
    free(lpf.data);

    return PASS;
}

int multi_tap_buff_tests()
{
    printf("\n");
    const int SIZE = 32;
    float *array = malloc(SIZE * sizeof(float));
    Mtap_buff input_buffer;
    float out = 0.0f;

    mtap_init(&input_buffer, array, SIZE);


    for (int i = 0; i < 250; i++) {
        mtap_update(&input_buffer, (float) i, &out);
    }

    print_float_array(stdout, input_buffer.buffer, input_buffer.size);

    free(array);

    return PASS;
}

int long_convolution()
{
    printf("\n");

    // segment size is size of sample chucks, power of 2
    // IR_SIZE size must be SEG_SIZE + 1 so that
    // dft_size = SEG_SIZE + IR_SIZE - 1 is a power of 2
    const int SEG_SIZE = 4096;
    const int IR_SIZE = SEG_SIZE + 1;
    const int DFT_SIZE = SEG_SIZE + IR_SIZE - 1;
    int num_conv;
    struct sample_data ir;
    float *ir_samples;

    Convolver *conv_engines;

    read_samples_from_wavfile("impulse_responses/nice_drum_room.wav", &ir);

    // calculate how many slices to chop IR into
    num_conv = ir.num_frames / (IR_SIZE);
    if ((ir.num_frames % (IR_SIZE)) != 0)
        num_conv++;

    // this array needs to be a length that is a multiple of IR_SIZE
    ir_samples = malloc(num_conv * (IR_SIZE) * sizeof(float));

    // copy interleved ir samples to array, and pad with zeros
    for (int i = 0; i < num_conv * IR_SIZE; i++) {
        if (i < ir.num_frames)
            ir_samples[i] = ir.frames[i].left;
        else
            ir_samples[i] = 0.0f;
    }

    // create convolution engines, each engine gets a slice of the IR
    conv_engines = malloc(num_conv * sizeof(Convolver));
    for (int i = 0; i < num_conv; i++) {
        init_convolver(&conv_engines[i], ir_samples + i * (IR_SIZE), IR_SIZE, SEG_SIZE);
    }

    // convolution algorithm using multi-tap buffer goes here

    // clean up
    for (int i = 0; i < num_conv; i++) {
        destroy_convolver(&conv_engines[i]);
    }
    free(conv_engines);
    free_sample_data(&ir);
    free(ir_samples);

    return PASS;
}

TESTS = {
    /*
    {"stereo audio test 1", playAudioStereo},
    {"wave file write test 1", writeSamplesToFile},
    {"fft", fft},
    {"low pass filter test", lowPassFilterWavefile},
    {"convolution testing", segmentedConvolution},
    {"multitap buffer testing", multi_tap_buff_tests},
    */
    {"long convolution", long_convolution},
    {0}
};
