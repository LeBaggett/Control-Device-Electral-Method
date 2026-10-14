#ifndef DEVICE_H
#define DEVICE_H
#include <stdint.h>
#include "linfit.h"

typedef enum {
    DEVICE_STANDBY = 0,
    DEVICE_RUNNING,
    DEVICE_RESULT,
    DEVICE_ERROR,
    DEVICE_ABORTED
} device_mode_t;

/* Estado compartilhado: a aplicação escreve; a UI consulta para desenhar. */
typedef struct {
    device_mode_t mode;
    float temperature_c;
    float target_temperature_c;
    float vf_voltage;
    uint8_t calibration_valid;
    uint8_t storage_ready;
    uint8_t progress_percent;
    uint32_t elapsed_seconds;
    int32_t error_code;
    uint32_t sensor_sequence;
    float vdda_v;
    uint8_t vdda_fallback;
    cal_params_t calibration;
    uint8_t calibrating;
} device_t;

#endif
