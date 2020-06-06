/* multi-tap ring buffer as described by:
 * https://archive.ednasia.com/www.ednasia.com/ART_8800522054_1000004_TA_61c5c459_2.HTM
 *
 * data is added to the buffer using the update method, which also returns the
 * oldest element in the buffer. Because this is essentially a ring buffer, the
 * oldest element is written over when a new one is added.
 *
 * */
#ifndef MTAP_BUFF_H
#define MTAP_BUFF_H

typedef struct mtap_buff {
    float *buffer;
    int size;
    int index;
} Mtap_buff;

/*
 * inits a multitap buffer with a user supplied array.
 * */
void mtap_init(Mtap_buff *b, float *array, int size);

/*
 * adds src to the buffer. src writes over the value returned in dest
 * If no return value is needed, set dest to NULL
 * */
void mtap_update(Mtap_buff *b, float src, float *dest);

/*
 * gets the element "tap" elements back from the newest element
 * i.e. tap = 0 points to the newest element, tap = 5 points to the element
 * added 5 updates ago
 * */
void mtap_get_at(Mtap_buff *b, int tap, float *dest);

/*
 * returns the array index of a given tap value.
 * if tap > array size, the index will wrap around.
 * */
int mtap_get_index(Mtap_buff *b, int tap);

#endif // MTAP_BUFF_H
