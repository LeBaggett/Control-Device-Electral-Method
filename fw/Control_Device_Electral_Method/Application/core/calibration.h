#ifndef THERMAL_CALIBRATION_H
#define THERMAL_CALIBRATION_H
#include "linfit.h"
void calibration_init(void);
const cal_params_t *calibration_get(void);
void calibration_set(const cal_params_t *cal);
#endif
