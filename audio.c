#include <stdlib.h>
#include <portaudio.h>
#include <sndfile.h>
#include <liquid/liquid.h>
#include "defs.h"

#define STACK_SIZE 128

static void *stack[STACK_SIZE] = {0};
static int stack_ptr = 0;

// TODO
// functions:
//  - fft
//  - decimate
//  - write filter to file
// 
// memory management:
//  - stack of pointers
//  - push for every malloc
//  - free everything in terminate_audio_systems()
//  - decide how to handle freeing and reallocating on a single pointer
static int INITIALIZED = 0;

void start_audio_systems()
{
        if (INITIALIZED)
                return;
        Pa_Initialize();
        INITIALIZED = 1;
        // TODO
        // setup a stack to hold pointers to memory allocated by the audio
        // functions.
}

void terminate_audio_systems()
{
        if (!INITIALIZED)
                return;
        Pa_Terminate();
        INITIALIZED = 0;
        // TODO
        // free all allocated memory.
        // this removes the need for the client code to call
        // free_sample_data(), free_filter_data(), etc.
}

static int audioCallback(const void *inputBuffer,
                void *outputBuffer,
                unsigned long framesPerBuffer,
                const PaStreamCallbackTimeInfo* timeInfo,
                PaStreamCallbackFlags statusFlag,
                void *sample_data)
{
        struct sample_data *data = (struct sample_data*) sample_data;
        float *out = (float*) outputBuffer;
        unsigned int i;

        for (i = 0; i < framesPerBuffer; i++) {
                if (data->index >= data->num_frames) {
                        return paComplete;
                }
                *out++ = data->frames[data->index].left;
                *out++ = data->frames[data->index].right;
                data->index++;
        }
        return 0;
}

int read_samples_from_wavfile(char *filename, struct sample_data *data)
{
        struct short_frame {
                short left;
                short right;
        } tmp;

        SNDFILE *f;
        SF_INFO info;

        info.format = 0;
        f = sf_open(filename, SFM_READ, &info);
        if (f == NULL) {
                printf("could not open %s\n", filename);
                return ERROR;
        }
        if (info.channels != 2) {
                printf("could not open %s.wav: file is not 2-channel\n",
                                filename);
                return ERROR;
        }
        data->frames = malloc(sizeof(struct float_frame) * info.frames);
        data->num_frames = info.frames;
        data->index = 0;
        data->sample_rate = info.samplerate;
        int i = 0;
        while (sf_readf_short(f, (short*) &tmp, 1) && i < data->num_frames) {
                data->frames[i].left = map((float) tmp.left,
                                -32768.0f, 32767.0f, -1.0f, 1.0f);
                data->frames[i].right = map((float) tmp.right,
                                -32768.0f, 32767.0f, -1.0f, 1.0f);
                i++;
        }
        sf_close(f);
        return SUCCESS;
}

void write_samples_to_wavfile(char *filename, struct sample_data *data)
{
        struct short_frame {
                short left;
                short right;
        } tmp;

        SF_INFO info;
        info.samplerate = data->sample_rate;
        info.channels = 2;
        info.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

        SNDFILE *f = sf_open(filename, SFM_WRITE, &info);
        if (f == NULL) {
                printf("could not open file %s\n", filename);
                return;
        }
        for (int i = 0; i < data->num_frames; i++) {
                // assumes samples are within range -1 to +1
                // may need to normalize
                tmp.left = (short) map(data->frames[i].left,
                                -1.0f, 1.0f, -32768.0f, 32767.0f);
                tmp.right = (short) map(data->frames[i].right,
                                -1.0f, 1.0f, -32768.0f, 32767.0f);
                sf_writef_short(f, (short*) &tmp, 1);
        }
        sf_close(f);
}

void free_sample_data(struct sample_data *data)
{
        if (data == NULL)
                return;
        free(data->frames);
        data->frames = NULL;
}

void play_audio_samples(struct sample_data *data)
{
        PaStream *stream;
        Pa_OpenDefaultStream(&stream, 0, 2, paFloat32, data->sample_rate,
                        256, audioCallback, data);
        Pa_StartStream(stream);
        while(Pa_IsStreamActive(stream)) {
                Pa_Sleep(100);
        }
        Pa_StopStream(stream);
        Pa_CloseStream(stream);
        data->index = 0;
}

void build_filter(struct impulse_response_data *filter, float f_c, float att)
{
        filter->num_points = estimate_req_filter_len(0.05, att);
        filter->data = malloc(sizeof(float) * filter->num_points);
        liquid_firdes_kaiser(filter->num_points, f_c, att, 0, filter->data);
        float sum = 0;
        for (int i = 0; i < filter->num_points; i++) {
                sum += filter->data[i];
        }
        filter->scale_factor = 1.0f / sum;
}

void apply_impulse_response(struct sample_data *data, struct impulse_response_data *filter)
{
        // need to convolve with unaltered samples
        float left_samples[data->num_frames];
        float right_samples[data->num_frames];

        for (int i = 0; i < data->num_frames; i++) {
            left_samples[i] = data->frames[i].left;
            right_samples[i] = data->frames[i].right;
        }

        for (int i = 0; i < data->num_frames; i++) {
                float left_result;
                float right_result;
                convolve(left_samples, data->num_frames, filter->data,
                                filter->num_points, i, &left_result);
                convolve(right_samples, data->num_frames, filter->data,
                                filter->num_points, i, &right_result);
                data->frames[i].left = left_result * filter->scale_factor;
                data->frames[i].right = right_result * filter->scale_factor;
        }
}

void read_coeff_from_file(char *path, struct impulse_response_data *data)
{
        FILE *in = fopen(path, "r");
        int size;
        int index;
        float value;

        fscanf(in, "# %d", &size);
        data->data = malloc(sizeof(float) * size);
        for (int i = 0; i < size; i++) {
                fscanf(in, "%d\t%f", &index, &value);
                data->data[index] = value;
        }
        fclose(in);
        data->num_points = size;
}

void free_filter_data(struct impulse_response_data *data)
{
        if (data == NULL)
                return;
        free(data->data);
        data->data = NULL;
}

void convert_wav_to_impulse_response(struct sample_data *wave, struct impulse_response_data *h)
{
    h->data = malloc(sizeof(float) * wave->num_frames);
    float sum = 0;
    for (int i = 0; i < wave->num_frames; i++) {
        h->data[i] = wave->frames[i].left;
        sum += wave->frames[i].left;
    }
    h->scale_factor = 1.0f / sum;
    h->num_points = wave->num_frames;
}
