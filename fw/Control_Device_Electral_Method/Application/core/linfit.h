#ifndef LINFIT_H
#define LINFIT_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
typedef struct { float m, t0, r2; uint16_t n; bool valid; } cal_params_t;
bool linfit(const float *vf, const float *temperature, size_t n, cal_params_t *out);
float cal_tj(const cal_params_t *cal, float vf);
#endif
