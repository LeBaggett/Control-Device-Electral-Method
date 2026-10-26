#include "format.h"
#include <math.h>
#include <stdio.h>
bool format_decimal(char *out, size_t size, float value, unsigned decimals)
{
    if (!out || size == 0) return false;
    out[0] = 0;
    if (!isfinite(value) || decimals > 6) return false;
    uint32_t scale = 1;
    for (unsigned i = 0; i < decimals; ++i) scale *= 10;
    double magnitude = fabs((double)value) * scale;
    if (magnitude > 2147483646.0) return false;
    uint32_t fixed = (uint32_t)(magnitude + 0.5);
    int count;
    const char *sign = value < 0 && fixed ? "-" : "";
    if (decimals)
        count = snprintf(out, size, "%s%lu.%0*lu", sign,
            (unsigned long)(fixed/scale), (int)decimals, (unsigned long)(fixed%scale));
    else count = snprintf(out, size, "%s%lu", sign, (unsigned long)fixed);
    return count >= 0 && (size_t)count < size;
}
uint32_t thermal_crc32(const uint8_t *data, size_t size)
{
    uint32_t crc = UINT32_MAX;
    for (size_t i = 0; i < size; ++i) {
        crc ^= data[i];
        for (unsigned bit = 0; bit < 8; ++bit)
            crc = (crc >> 1) ^ ((crc & 1U) ? 0xEDB88320U : 0U);
    }
    return ~crc;
}
