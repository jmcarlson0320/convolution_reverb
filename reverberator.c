#include "defs.h"
#include "mtap_buff.h"
#include "fft_convolve.h"

static const int SEG_SIZE = 4096;
static const int IR_SIZE = SEG_SIZE + 1;

static int calc_num_segs(int length, int seg_size)
{
    int num_segs;

    num_segs = length / seg_size;
    if ((length % seg_size) != 0)
        num_segs++;

    return num_segs;
}

void reverberate(struct sample_data *impulse_response, struct sample_data *input, struct sample_data *output)
{
    float *ir_samples;
    float *input_samples;
    float *output_samples;
    float accumulator[SEG_SIZE];
    int num_convolvers;
    int num_sample_blocks;
    float *buffer;
    Mtap_buff input_buffer;
    Convolver *convolution_engines;

    // setup ir
    num_convolvers = calc_num_segs(impulse_response->num_frames, IR_SIZE);
    ir_samples = malloc(num_convolvers * IR_SIZE * sizeof(float));
    get_left_channel(impulse_response, ir_samples, num_convolvers * IR_SIZE);
    
    // setup convolution engines
    convolution_engines = malloc(num_convolvers * sizeof(Convolver));
    for (int i = 0; i < num_convolvers; i++) {
        float *offset_into_ir = ir_samples + i * IR_SIZE;
        init_convolver(&convolution_engines[i], offset_into_ir, IR_SIZE, SEG_SIZE);
    }

    // setup sample buffers
    num_sample_blocks = calc_num_segs(input->num_frames, SEG_SIZE);
    input_samples = malloc(num_sample_blocks * SEG_SIZE * sizeof(float));
    output_samples = malloc(num_sample_blocks * SEG_SIZE * sizeof(float));
    get_left_channel(input, input_samples, num_sample_blocks * SEG_SIZE);

    // setup multi-tap input buffer
    buffer = malloc(num_convolvers * SEG_SIZE * sizeof(float));
    clear_float_array(buffer, num_convolvers * SEG_SIZE, 0.0f);
    mtap_init(&input_buffer, buffer, num_convolvers * SEG_SIZE);

    // run convolution
    for (int cur_block = 0; cur_block < num_sample_blocks; cur_block++) {
        // write current input block to buffer
        for (int i = 0; i < SEG_SIZE; i++) {
            mtap_update(&input_buffer, input_samples[i + cur_block * SEG_SIZE], NULL);
        }

        // clear accumulator
        clear_float_array(accumulator, SEG_SIZE, 0.0f);

        // run convolution engines
        for (int i = 0; i < num_convolvers; i++) {
            int delay = (i + 1) * SEG_SIZE - 1;
            int tap = mtap_get_index(&input_buffer, delay);
            float tmp[SEG_SIZE];

            fft_convolve(&convolution_engines[i], input_buffer.buffer + tap, tmp);

            // sum results
            for (int j = 0; j < SEG_SIZE; j++) {
                accumulator[j] += tmp[j];
            }
        }

        // write sum to proper output block
        float scale_factor = 1.0f / num_convolvers * 0.8f;
        for (int i = 0; i < SEG_SIZE; i++) {
            output_samples[i + cur_block * SEG_SIZE] = accumulator[i] * scale_factor;
        }
    }

    // setup output
    output->frames = malloc(num_sample_blocks * SEG_SIZE * sizeof(struct float_frame));
    output->num_frames = num_sample_blocks * SEG_SIZE;
    output->index = 0;
    output->sample_rate = input->sample_rate;

    // copy to output
    for (int i = 0; i < num_sample_blocks * SEG_SIZE; i++) {
        output->frames[i].left = output_samples[i];
        output->frames[i].right = output_samples[i];
    }
}
