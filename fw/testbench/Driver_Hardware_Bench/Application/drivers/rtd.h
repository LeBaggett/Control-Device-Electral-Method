#ifndef RTD_H
#define RTD_H
#include <stdbool.h>
/* Nome legado mantido para compatibilidade: usa MAX31855/termopar. */
void rtd_init(void);
bool rtd_read_temperature(float *temperature_c);
#endif
