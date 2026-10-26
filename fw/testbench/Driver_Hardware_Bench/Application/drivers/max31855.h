#ifndef MAX31855_H
#define MAX31855_H
#include <stdbool.h>
#include <stdint.h>
typedef enum {
    MAX31855_OK = 0,
    MAX31855_OPEN_CIRCUIT,
    MAX31855_SHORT_GND,
    MAX31855_SHORT_VCC,
    MAX31855_FRAME_ERROR,
    MAX31855_SPI_ERROR
} max31855_status_t;
void max31855_init(void);
max31855_status_t max31855_decode(uint32_t raw, float *thermocouple_c,
                                  float *internal_c);
max31855_status_t max31855_read(float *thermocouple_c);
#endif
