#include "defs.h"

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
        return FAILURE;
    if (n >= len)
        return FAILURE;

    float sum = 0;
    for (int k = n - M - 1; k <= n; k++) {
        if (k < 0)
            continue;
        sum += x[k] * h[n - k];
    }
    *y_n = sum;

    return SUCCESS;
}
