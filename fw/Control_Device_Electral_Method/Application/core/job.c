#include "job.h"
#include "targets.h"
#include "linfit.h"
#include "calibration.h"
#include "config.h"
#include "board.h"
#include "power.h"
#include "vf_adc.h"
#include "sdlog.h"
#include "format.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
typedef enum { WAIT_TARGET, SOAK, CAL_SAMPLE, HEAT_SAMPLE, STORE_SAMPLE } phase_t;
static struct {
    sweep_cfg_t cfg;
    job_kind_t kind;
    job_status_t status;
    job_error_t error;
    phase_t phase;
    unsigned index, points, faults;
    uint32_t started_ms, target_ms, stable_ms, sample_ms, sensor_sequence;
    float target, sampled_temp, sampled_vf;
    float temperatures[TARGETS_MAX], voltages[TARGETS_MAX];
    bool paused, session_open;
} job;
job_error_t job_error(void) { return job.error; }
job_kind_t job_kind(void) { return job.kind; }
void job_init(void) { memset(&job,0,sizeof(job)); job.status = JOB_STATUS_IDLE; }
static void stop(void)
{
    power_safe(); vf_adc_cancel_samples();
    if (job.session_open) { (void)sdlog_close(); job.session_open = false; }
}
static job_status_t fail(job_error_t error, device_t *d)
{
    job.error = error; job.status = JOB_STATUS_FAILED;
    if (d) d->error_code = error;
    stop(); return job.status;
}
static bool start(job_kind_t kind, float first, float last, float step)
{
    sweep_cfg_t cfg = {first,last,step};
    size_t points = targets_count(&cfg);
    if (job.status == JOB_STATUS_RUNNING || points == 0) {
        job.error = JOB_ERROR_INVALID_PARAMS; return false;
    }
    if (kind == JOB_KIND_CALIBRATION && points < 2) {
        job.error = JOB_ERROR_FIT; return false;
    }
    if (kind == JOB_KIND_TEST && REQUIRE_CAL && !calibration_get()->valid) {
        job.error = JOB_ERROR_NO_CALIBRATION; return false;
    }
    job_init(); job.kind = kind; job.cfg = cfg; job.points = (unsigned)points;
    job.target = first;
    if (!sdlog_ready() || !sdlog_open_run(kind == JOB_KIND_CALIBRATION)) {
        (void)fail(JOB_ERROR_STORAGE,NULL); return false;
    }
    job.session_open = true;
    if (!vf_adc_ready()) { (void)fail(JOB_ERROR_SAMPLE_FAILED,NULL); return false; }
    /* SELECT measurement BEFORE relay, mandatory for both kinds. */
    if (!power_enable_measurement() || !power_relay_on()) {
        (void)fail(JOB_ERROR_POWER_LOCKED,NULL); return false;
    }
    if (kind == JOB_KIND_TEST && !power_select_heating()) {
        (void)fail(JOB_ERROR_POWER_LOCKED,NULL); return false;
    }
    job.started_ms = job.target_ms = board_millis();
    job.sensor_sequence = UINT32_MAX; job.phase = WAIT_TARGET;
    job.status = JOB_STATUS_RUNNING;
    return true;
}
bool job_start_calibration_params(float first,float last,float step)
{ return start(JOB_KIND_CALIBRATION,first,last,step); }
bool job_start_calibration(void) { return job_start_calibration_params(25,85,10); }
bool job_start_test(float first,float last,float step)
{ return start(JOB_KIND_TEST,first,last,step); }
void job_abort(void)
{
    if (job.status == JOB_STATUS_RUNNING) { job.status = JOB_STATUS_ABORTED; stop(); }
}
static bool make_line(char *line, size_t size)
{
    char target[24], temp[24], vf[24], tj[24], time[24];
    if (!format_decimal(target,sizeof(target),job.target,2) ||
        !format_decimal(temp,sizeof(temp),job.sampled_temp,2) ||
        !format_decimal(vf,sizeof(vf),job.sampled_vf,5)) return false;
    int n;
    if (job.kind == JOB_KIND_CALIBRATION)
        n = snprintf(line,size,"%u,%s,%s,%s",job.index,target,temp,vf);
    else {
        if (!format_decimal(tj,sizeof(tj),cal_tj(calibration_get(),job.sampled_vf),2))
            (void)snprintf(tj,sizeof(tj),"NaN");
        if (!format_decimal(time,sizeof(time),(float)(job.sample_ms-job.started_ms)/1000.0f,3))
            return false;
        n = snprintf(line,size,"%u,%s,%s,%s,%s,%s",job.index,target,temp,vf,tj,time);
    }
    return n >= 0 && (size_t)n < size;
}
job_status_t job_step(device_t *d)
{
    if (job.status != JOB_STATUS_RUNNING) return job.status;
    if (!d) return fail(JOB_ERROR_SAMPLE_FAILED,NULL);
    uint32_t now = board_millis();
    d->target_temperature_c = job.target;
    d->elapsed_seconds = (uint32_t)(now-job.started_ms)/1000U;
    d->progress_percent = (uint8_t)(job.index*100U/job.points);
    uint32_t timeout = job.kind == JOB_KIND_CALIBRATION ? CAL_WAIT_TIMEOUT_S : TEST_TARGET_TIMEOUT_S;
    if ((uint32_t)(now-job.target_ms) >= timeout*1000U)
        return fail(JOB_ERROR_TARGET_TIMEOUT,d);
    if (!sdlog_ready()) return fail(JOB_ERROR_STORAGE,d);
    bool fresh = d->sensor_sequence != job.sensor_sequence;
    if (fresh) {
        job.sensor_sequence = d->sensor_sequence;
        if (!isfinite(d->temperature_c)) {
            /* Turn power off at FIRST bad sample; count only distinct reads. */
            power_safe(); job.paused = true;
            vf_adc_cancel_samples();
            if (++job.faults >= RTD_FAULT_LIMIT) return fail(JOB_ERROR_SENSOR,d);
            return job.status;
        }
        if (d->temperature_c >= OVERTEMP_C) return fail(JOB_ERROR_OVERTEMP,d);
        job.faults = 0;
        if (job.paused) {
            if (!power_enable_measurement() || !power_relay_on() ||
                (job.kind == JOB_KIND_TEST && !power_select_heating()))
                return fail(JOB_ERROR_POWER_LOCKED,d);
            job.paused = false; job.phase = WAIT_TARGET;
        }
    }
    if (job.paused || !isfinite(d->temperature_c)) return job.status;
    switch (job.phase) {
    case WAIT_TARGET:
        if (!fresh) break;
        if (job.kind == JOB_KIND_CALIBRATION) {
            if (fabsf(d->temperature_c-job.target) <= CAL_TOL_C) {
                job.stable_ms = now; job.phase = SOAK;
            }
        } else if (d->temperature_c >= job.target) job.phase = HEAT_SAMPLE;
        break;
    case SOAK:
        if (!fresh) break;
        if (fabsf(d->temperature_c-job.target) > CAL_TOL_C) job.phase = WAIT_TARGET;
        else if ((uint32_t)(now-job.stable_ms) >= CAL_SOAK_S*1000U) {
            if (!vf_adc_begin_samples(CAL_VF_SAMPLES)) return fail(JOB_ERROR_SAMPLE_FAILED,d);
            job.sample_ms = now; job.phase = CAL_SAMPLE;
        }
        break;
    case CAL_SAMPLE:
        if (fabsf(d->temperature_c-job.target) > CAL_TOL_C)
            return fail(JOB_ERROR_SAMPLE_FAILED,d);
        if (!vf_adc_take_samples(&job.sampled_vf)) {
            if ((uint32_t)(now-job.sample_ms) > 1000U) return fail(JOB_ERROR_SAMPLE_FAILED,d);
            break;
        }
        if (!isfinite(job.sampled_vf)) return fail(JOB_ERROR_SAMPLE_FAILED,d);
        job.sampled_temp = d->temperature_c; job.sample_ms = now; job.phase = STORE_SAMPLE;
        break;
    case HEAT_SAMPLE:
        /* No OLED/FatFs calls inside the SELECT -> delay -> ADC window.
         * Ordering follows BPMN and detailed test: SELECT FIRST, then 30us. */
        if (!power_enable_measurement()) return fail(JOB_ERROR_POWER_LOCKED,d);
        board_delay_us(TEST_MEAS_DELAY_US);
        if (!board_is_ready() || !vf_adc_capture(TEST_VF_SAMPLES,&job.sampled_vf) ||
            !isfinite(job.sampled_vf)) return fail(JOB_ERROR_SAMPLE_FAILED,d);
        job.sampled_temp = d->temperature_c; job.sample_ms = board_millis();
        /* Keep measurement while storing; do not heat through blocking SD I/O. */
        job.phase = STORE_SAMPLE;
        break;
    case STORE_SAMPLE: {
        char line[160];
        if (!make_line(line,sizeof(line)) || !sdlog_line(line)) return fail(JOB_ERROR_STORAGE,d);
        job.temperatures[job.index] = job.sampled_temp;
        job.voltages[job.index] = job.sampled_vf;
        d->vf_voltage = job.sampled_vf;
        if (++job.index == job.points) {
            power_safe();
            bool closed = sdlog_close(); job.session_open = false;
            if (!closed) return fail(JOB_ERROR_FINALIZE_FAILED,d);
            if (job.kind == JOB_KIND_CALIBRATION) {
                cal_params_t result;
                if (!linfit(job.voltages,job.temperatures,job.points,&result))
                    return fail(JOB_ERROR_FIT,d);
                if (!sdlog_save_calibration(&result)) return fail(JOB_ERROR_FINALIZE_FAILED,d);
                calibration_set(&result); d->calibration = result; d->calibration_valid = 1;
            }
            d->progress_percent = 100; job.status = JOB_STATUS_COMPLETED;
        } else {
            job.target = targets_at(&job.cfg,job.index); job.target_ms = board_millis();
            job.phase = WAIT_TARGET;
            if (job.kind == JOB_KIND_TEST && !power_select_heating())
                return fail(JOB_ERROR_POWER_LOCKED,d);
        }
        break;
    }
    }
    return job.status;
}
