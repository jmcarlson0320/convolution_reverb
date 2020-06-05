#include "defs.h"
#include "mtap_buff.h"
#include "fft_convolve.h"

static const int SEG_SIZE = 4096;
static const int IR_SIZE = SEG_SIZE + 1;
static const int DFT_SIZE = SEG_SIZE  + IR_SIZE - 1;

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

    // setup input/out
    num_sample_blocks = calc_num_segs(input->num_frames, SEG_SIZE);
    input_samples = malloc(num_sample_blocks * SEG_SIZE * sizeof(float));
    output_samples = malloc(num_sample_blocks * SEG_SIZE * sizeof(float));
    get_left_channel(input, input_samples, num_sample_blocks * SEG_SIZE);

    // setup multi-tap input buffer
    buffer = malloc(num_convolvers * SEG_SIZE * sizeof(float));
    clear_float_array(buffer, num_convolvers * SEG_SIZE, 0.0f);
    mtap_init(&input_buffer, buffer, num_convolvers * SEG_SIZE);


    // run convolution

    // copy to output
}
