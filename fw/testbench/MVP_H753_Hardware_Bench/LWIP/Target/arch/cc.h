#ifndef BENCH_LWIP_CC_H
#define BENCH_LWIP_CC_H
#include <stdint.h>
#include "main.h"
#define LWIP_PLATFORM_DIAG(x) do {} while(0)
#define LWIP_PLATFORM_ASSERT(x) do {(void)(x);bench_fatal(110);}while(0)
static inline uint32_t bench_random(void){static uint32_t state=0x51A75C3DU;state=state*1664525U+1013904223U;return state;}
#define LWIP_RAND() bench_random()
#endif
