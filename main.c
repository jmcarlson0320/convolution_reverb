#include <stdio.h>

void usage()
{
    printf("usage: reverb audiofile.wav\n");
}

int main(int argc, char *argv[])
{
    char *input_file;
    char *output_file;

    if (argc !=  2) {
        usage();
        return 1;
    }
    input_file = argv[1];

    return 0;
}
