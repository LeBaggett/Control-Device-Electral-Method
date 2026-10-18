#ifndef JOB_H
#define JOB_H
#include <stdbool.h>
#include "device.h"

typedef enum {
    JOB_STATUS_IDLE = 0,
    JOB_STATUS_RUNNING,
    JOB_STATUS_COMPLETED,
    JOB_STATUS_FAILED,
    JOB_STATUS_ABORTED
} job_status_t;

typedef enum {
    JOB_KIND_CALIBRATION = 0,
    JOB_KIND_TEST
} job_kind_t;

typedef enum {
    JOB_ERROR_NONE = 0,
    JOB_ERROR_INVALID_PARAMS = 1,
    JOB_ERROR_BACKEND_UNAVAILABLE = 2,
    JOB_ERROR_TARGET_TIMEOUT = 3,
    JOB_ERROR_SAMPLE_FAILED = 4,
    JOB_ERROR_FINALIZE_FAILED = 5,
    JOB_ERROR_STORAGE = 6,
    JOB_ERROR_SENSOR = 7,
    JOB_ERROR_OVERTEMP = 8,
    JOB_ERROR_FIT = 9,
    JOB_ERROR_NO_CALIBRATION = 10,
    JOB_ERROR_POWER_LOCKED = 11
} job_error_t;

typedef struct {
 bool enabled, reference_valid;
 float slope_c_per_v, reference_c, reference_vf, last_tj_c;
} manual_k_debug_t;
extern volatile manual_k_debug_t g_manual_k_debug;
bool job_start_test_manual_k(float start_c,float end_c,float step_c,float slope_c_per_v,float reference_c);
void job_init(void);
bool job_start_calibration(void);
bool job_start_calibration_params(float start_c, float end_c, float step_c);
bool job_start_test(float start_temperature_c, float end_temperature_c,
                    float step_temperature_c);
void job_abort(void);
job_status_t job_step(device_t *device);
job_error_t job_error(void);
job_kind_t job_kind(void);
#endif
