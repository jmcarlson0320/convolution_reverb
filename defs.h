#ifndef DEFS_H
#define DEFS_H

#define SUCCESS 1
#define ERROR 0
#define NUM_CHANNELS 2

struct float_frame {
        float left;
        float right;
};

struct sample_data {
        struct float_frame *frames;
        int num_frames;
        int index;
        int sample_rate;
};

struct impulse_response_data {
        float *data;
        int num_points;
        float scale_factor; // recip of sum of coefficients, need to think about this more
};

// audio.c
void start_audio_systems();
void terminate_audio_systems();
int read_samples_from_wavfile(char *filename, struct sample_data *data);
void write_samples_to_wavfile(char *filename, struct sample_data *data);
void free_sample_data(struct sample_data *data);
void play_audio_samples(struct sample_data *data);
void build_filter(struct impulse_response_data *filter, float f_c, float att);
void apply_impulse_response(struct sample_data *data, struct impulse_response_data *filter);
void read_coeff_from_file(char *path, struct impulse_response_data *coeff);
void write_coeff_to_file(char *path, struct impulse_response_data *data);

// utils.c
int convolve(float *x, int len, float *h, int M, int n, float *y_n);
float lerp(float norm, float min, float max);
float norm(float value, float min, float max);
float map(float value, float srcMin, float srcMax, float destMin, float destMax);

#endif // DEFS_H
