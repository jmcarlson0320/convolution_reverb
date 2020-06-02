/* multi-tap ring buffer as described by:
 * https://archive.ednasia.com/www.ednasia.com/ART_8800522054_1000004_TA_61c5c459_2.HTM
 * */
#ifndef MTAP_BUFF_H
#define MTAP_BUFF_H

typedef struct mtap_buff {
    float *buffer;
    int size;
    int index;
} Mtap_buff;

void mtap_init(Mtap_buff *b, float *array, int size);
void mtap_update(Mtap_buff *b, float src, float *dest);
void mtap_get_at(Mtap_buff *b, int tap, float *dest);
int mtap_get_index(Mtap_buff *b, int tap);

#endif // MTAP_BUFF_H
