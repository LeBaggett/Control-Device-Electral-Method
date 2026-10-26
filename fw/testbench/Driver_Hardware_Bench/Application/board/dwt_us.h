#ifndef DWT_US_H
#define DWT_US_H
#include <stdbool.h>
#include <stdint.h>
bool DWT_US_Init(void);
/* Bounded 0..1000 us; false on counter/clock failure. Thread context only.
 * Reinitialize after a core-clock change. Not a wall-clock across debug halt. */
bool DWT_US_Delay(uint32_t us);
#endif
