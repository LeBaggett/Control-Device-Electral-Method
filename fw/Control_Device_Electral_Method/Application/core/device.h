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
    float tc_gain, tc_r2;
    uint8_t tc_points, tc_required, tc_capture_count, tc_capture_busy, tc_candidate_valid;
    float tc_offset_c;
    int32_t tc_adjust_status; /* 0=idle, 1=saved, 2=invalid/stale reference, 3=SD error */
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
    uint8_t manual_k_active;
    uint8_t tc_heat_active,tc_heat_stable,tc_heat_error,tc_heat_holding,tc_heat_waiting_next;
    float tc_heat_target_c;
    uint8_t power_control_enabled;
} device_t;

#endif
