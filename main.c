#include <stdio.h>
#include <string.h>
#include "defs.h"

enum RESPONSE {
    LARGE_DAMPED,
    DEEP_SPACE,
    ECHO,
    LONG,
    MASONIC_LODGE,
    DARK,
    MUSIKVSL,
    DRUM_ROOM,
    STAR,
    GARAGE,
    OPERA_HOUSE,
    SMALL_CAVE,
    CHURCH,
    NUM_RESPONSES
};

char *ir_names[NUM_RESPONSES] = {
    [LARGE_DAMPED] = "large_damped",
    [DEEP_SPACE] = "deep_space",
    [ECHO] = "echo",
    [LONG] = "long",
    [MASONIC_LODGE] = "masonic_lodge",
    [DARK] = "dark",
    [MUSIKVSL] = "musikvsl",
    [DRUM_ROOM] = "drum_room",
    [STAR] = "star",
    [GARAGE] = "garage",
    [OPERA_HOUSE] = "opera_house",
    [SMALL_CAVE] = "small_cave",
    [CHURCH] = "church"
};

char *ir_files[NUM_RESPONSES] = {
    [LARGE_DAMPED] = "impulse_responses/damped_lg_room.wav",
    [DEEP_SPACE] = "impulse_responses/deep_space.wav",
    [ECHO] = "impulse_responses/echo_hall.wav",
    [LONG] = "impulse_responses/five_columns_long.wav",
    [MASONIC_LODGE] = "impulse_responses/masonic_lodge.wav",
    [DARK] = "impulse_responses/med_dark.wav",
    [MUSIKVSL] = "impulse_responses/musikvereinsaal.wav",
    [DRUM_ROOM] = "impulse_responses/nice_drum_room.wav",
    [STAR] = "impulse_responses/on_a_star.wav",
    [GARAGE] = "impulse_responses/parking_garage.wav",
    [OPERA_HOUSE] = "impulse_responses/scala_milan_opera_house.wav",
    [SMALL_CAVE] = "impulse_responses/small_cave.wav",
    [CHURCH] = "impulse_responses/st_nicolas_church.wav"
};

enum {
    OUTPUT_FILE_SPECIFIED,
    IR_SPECIFIED,
    LIST_AVAILABLE_IR,
    NUM_MODES
};

const enum RESPONSE default_response = LONG;

void usage()
{
    printf("usage: reverb [-i impulseresponse] [-o outputfile] inputfile\n");
    printf("OR: reverb -l (lists available impulse responses)\n");
}

void list_responses()
{
    for (int i = 0; i < NUM_RESPONSES; i++) {
        printf("%s\n", ir_names[i]);
    }
}

int main(int argc, char *argv[])
{
    char *input_file = NULL;
    char *output_file = NULL;
    char *ir = NULL;
    int index = 1;
    int options[NUM_MODES] = {0};

    struct sample_data ir_data;
    struct sample_data input_data;
    struct sample_data output_data;

    // parse arguments
    while (index < argc && argv[index][0] == '-') {
        switch (argv[index][1]) {
            case 'o':
                // set output file
                if (++index < argc) {
                    options[OUTPUT_FILE_SPECIFIED] = 1;
                    output_file = argv[index];
                } else {
                    usage();
                    exit(0);
                }
                break;
            case 'i':
                // set impulse response
                if (++index < argc) {
                    options[IR_SPECIFIED] = 1;
                    ir = argv[index];
                } else {
                    usage();
                    exit(0);
                }
                break;
            case 'l':
                // print available inpulse responses
                options[LIST_AVAILABLE_IR] = 1;
                break;
            default:
                usage();
                exit(0);
                break;
        }
        index++;
    }

    if (options[LIST_AVAILABLE_IR]) {
        list_responses();
        return 0;
    }

    // the last argument is the input file
    // set input file
    if (index < argc) {
        input_file = argv[index];
    } else {
        usage();
        exit(0);
    }

    // load input file
    if (read_samples_from_wavfile(input_file, &input_data) == ERROR) {
        exit(0);
    }

    // load impulse response
    int IR_CODE = default_response;
    int found = 0;
    if (options[IR_SPECIFIED]) {
        int i = 0;
        while (i < NUM_RESPONSES && !found) {
            if (strcmp(ir_names[i], ir) == 0) {
                found = 1;
                IR_CODE = i;
            }
            i++;
        }
        if (!found) {
            printf("impulse_response %s not found\n", ir);
            printf("using default response: %s\n", ir_names[IR_CODE]);
        } else {
            printf("using response: %s\n", ir_names[IR_CODE]);
        }
    } else {
        printf("using default response: %s\n", ir_names[IR_CODE]);
    }

    if (read_samples_from_wavfile(ir_files[IR_CODE], &ir_data) == ERROR) {
        exit(0);
    }

    // apply convolution reverb
    reverberate(&ir_data, &input_data, &output_data);


    // play result or write to file
    if (options[OUTPUT_FILE_SPECIFIED]) {
        write_samples_to_wavfile(output_file, &output_data);
    } else {
        start_audio_systems();
        play_audio_samples(&output_data);
        terminate_audio_systems();
    }

    return 0;
}
