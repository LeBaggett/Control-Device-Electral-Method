#ifndef THERMAL_FORMAT_H
#define THERMAL_FORMAT_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
/* Fixed decimals without printf float support; reject overflow/NaN/Inf. */
bool format_decimal(char *out, size_t size, float value, unsigned decimals);
uint32_t thermal_crc32(const uint8_t *data, size_t size);
#endif
