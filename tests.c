#include "unit_test.h"
#include "defs.h"

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

int impulseTest()
{
        struct sample_data data;
        struct sample_data impulse_response;
        int result;

        read_samples_from_wavfile("audio_files/piano2.wav", &data);
        read_samples_from_wavfile("impulse_responses/five_columns_long.wav", &impulse_response);

        start_audio_systems();
        play_audio_samples(&data);
        play_audio_samples(&impulse_response);
        terminate_audio_systems();

        return PASS;
}

TESTS = {
    {"stereo audio test 1", playAudioStereo},
    {"wave file write test 1", writeSamplesToFile},
    {"low pass filter test", lowPassFilterWavefile},
    {"load impulse_response tests", impulseTest},
    {0}
};
