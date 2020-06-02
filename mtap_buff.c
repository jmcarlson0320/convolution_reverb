#include "mtap_buff.h"

void mtap_init(Mtap_buff *b, float *array, int size)
{
    b->buffer = array;
    b->size = size;
    b->index = 0;
}

void mtap_update(Mtap_buff *b, float src, float *dest)
{
    int i = b->index;

    *dest = b->buffer[i];
    b->buffer[i] = src;

    b->index++;
    if (b->index >= b->size)
        b->index = 0;
}

void mtap_get_at(Mtap_buff *b, int tap, float *dest)
{
    int i = mtap_get_index(b, tap);

    *dest = b->buffer[i];
}

// index points to the oldest element
// index - 1 points to the newest element
// tap specifies number of elements back from newest
// i.e. tap 0 = newest element (index - 1)
//      tap 5 = newest element - 5 (index - 6)
int mtap_get_index(Mtap_buff *b, int tap)
{
    int i = b->index;
    int N = b->size;

    return (i - (1 + tap) + N) % N;
}
