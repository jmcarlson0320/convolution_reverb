#include "defs.h"

#include <math.h>

float lerp(float norm, float min, float max)
{
    return (max - min) * norm + min;
}

float norm(float value, float min, float max)
{
    return (value - min) / (max - min);
}

float map(float value, float srcMin, float srcMax, float destMin, float destMax)
{
    float n = norm(value, srcMin, srcMax);
    return lerp(n, destMin, destMax);
}

int convolve(float *x, int len, float *h, int M, int n, float *y_n)
{
    if (!x || !h || !y_n)
        return ERROR;
    if (n >= len + M - 1)
        return ERROR;

    float sum = 0;
    for (int k = n - M - 1; k <= n; k++) {
        if (k < 0 || len <= k)
            continue;
        else
            sum += x[k] * h[n - k];
    }
    *y_n = sum;

    return SUCCESS;
}

void print_float_array(FILE *out, float *array, int n)
{
    for (int i = 0; i < n; i++) {
        fprintf(out, "%6.12f\n", array[i]);
    }
}

void print_real_part(FILE *out, fftw_complex *array, int n)
{
    for (int i = 0; i < n; i++) {
        fprintf(out, "%6.12f\n", array[i].re);
    }
}

void print_complex_array_mag(FILE *out, fftw_complex *array, int n)
{
    for (int i = 0; i < n; i++) {
        float re = array[i].re;
        float im = array[i].im;
        float mag = sqrt(re * re + im * im);
        fprintf(out, "%6.12f\n", mag);
    }
}

fftw_complex complex_multiply(fftw_complex x, fftw_complex y)
{
    fftw_complex result;

    result.re = x.re * y.re - x.im * y.im;
    result.im = x.re * y.im + x.im * y.re;

    return result;
}

void clear_float_array(float *a, int len, float val)
{
    if (!a)
        return;

    for (int i = 0; i < len; i++) {
        a[i] = val;
    }
}
