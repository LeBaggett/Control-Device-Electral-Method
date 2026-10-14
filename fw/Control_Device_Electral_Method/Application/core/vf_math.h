#ifndef VF_MATH_H
#define VF_MATH_H
#include <stdint.h>
float vf_from_counts(float average, float vdda);
float vdda_from_vrefint(uint16_t calibration, uint16_t raw);
#endif
