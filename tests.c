#include "unit_test.h"
#include "defs.h"

int playAudioStereo()
{
        struct sample_data data;

        start_audio_systems();
        read_samples_from_wavfile("piano2.wav", &data);
        play_audio_samples(&data);
        terminate_audio_systems();
        free_sample_data(&data);
        return PASS;
}

int writeSamplesToFile()
{
        struct sample_data data;

        read_samples_from_wavfile("piano2.wav", &data);
        write_samples_to_wavfile("out.wav", &data);
        free_sample_data(&data);
        return PASS;
}

TESTS = {
        {"stereo audio test 1", playAudioStereo},
        {"wave file write test 1", writeSamplesToFile},
        {0}
};
