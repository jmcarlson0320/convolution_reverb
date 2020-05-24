#include <stdio.h>

void usage()
{
        printf("usage: reverb [-oi] [outputfile|impulseresponse] inputfile\n");
        printf("usage: reverb -l (list available impulse responses)\n");
}

int main(int argc, char *argv[])
{
        enum {
                OUTPUT_FILE_SPECIFIED,
                IR_SPECIFIED,
                LIST_AVAILABLE_IR,
                NUM_MODES
        };

        char *input_file = NULL;
        char *output_file = NULL;
        int index = 1;
        int options[NUM_MODES] = {0};

        while (index < argc && argv[index][0] == '-') {
                switch (argv[index][1]) {
                        case 'o':
                                // set output file
                                if (++index < argc) {
                                        options[OUTPUT_FILE_SPECIFIED] = 1;
                                        output_file = argv[index];
                                }
                                break;
                        case 'i':
                                // set impulse response
                                break;
                        case 'l':
                                // print available inpulse responses
                                break;
                        default:
                                usage();
                                break;
                }
                index++;
        }

        if (index < argc) {
                input_file = argv[index];
        } else {
                usage();
        }

        printf("input file: %s\n", input_file);
        printf("output file: %s\n", output_file);

        return 0;
}
