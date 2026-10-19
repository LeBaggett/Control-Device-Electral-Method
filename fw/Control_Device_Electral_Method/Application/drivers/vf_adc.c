#include "vf_adc.h"
#include "peripheral_io.h"
#include "board.h"
#include "runtime_config.h"
#include "vf_math.h"
#include <math.h>
#include <stdint.h>
volatile vf_adc_debug_t g_vf_debug;
static bool ready, valid, fallback, waiting, measuring_vref, request, completed;
static uint32_t sum, count, wanted, sample_ms, conversion_ms, vref_ms;
static float voltage, vdda;
static uint16_t reference;
static float convert(float raw)
{
    return (vf_from_counts(raw,vdda)-THERMAL_ADC_OFFSET_V)/THERMAL_ADC_GAIN;
}
void vf_adc_init(void)
{
    ready = valid = waiting = request = completed = false;
    sum = count = 0; wanted = THERMAL_ADC_AVERAGE_COUNT; voltage = NAN;
    vdda = VDDA_NOMINAL_V; fallback = true; vref_ms = board_millis();
    if (!THERMAL_ADC_CONFIRMED || !isfinite(THERMAL_ADC_GAIN) ||
        THERMAL_ADC_GAIN <= 0 || !isfinite(THERMAL_ADC_OFFSET_V)) return;
    reference = board_vrefint_calibration();
    ready = board_adc_start(); measuring_vref = true;
}
bool vf_adc_ready(void) { return ready; }
void vf_adc_cancel_samples(void)
{
    if (ready) board_adc_cancel();
    request = completed = waiting = false;
    wanted = THERMAL_ADC_AVERAGE_COUNT; sum = count = 0;
}
float vf_adc_vdda(void) { return vdda; }
bool vf_adc_vdda_fallback(void) { return fallback; }
bool vf_adc_refresh_reference(void)
{
    g_vf_debug.reference_refreshes++;
    if (!ready) {g_vf_debug.reference_status=1;return false;}
    if (request) {g_vf_debug.reference_status=2;return false;}
    board_adc_cancel();waiting=false;sum=count=0;
    valid=false;voltage=NAN;measuring_vref=true;
    if (!board_adc_begin(true)) {g_vf_debug.reference_status=3;return false;}
    uint32_t started=board_millis(),guard=100000U;
    uint16_t raw;
    while (!board_adc_sample(&raw)) {
        if (--guard==0 || (uint32_t)(board_millis()-started)>=5U) {
            board_adc_cancel();g_vf_debug.reference_status=4;return false;
        }
    }
    if(raw==0 || raw>=4095) {g_vf_debug.reference_status=5;return false;}
    float estimate=vdda_from_vrefint(reference,raw);
    fallback=!isfinite(estimate);vdda=fallback?VDDA_NOMINAL_V:estimate;
    vref_ms=board_millis();measuring_vref=false;
    g_vf_debug.reference_age_ms=0;g_vf_debug.reference_status=0;
    return true;
}
bool vf_adc_begin_samples(unsigned n)
{
    if (!ready || request || n == 0 || n > 64) return false;
    board_adc_cancel(); waiting = false; measuring_vref = true;
    wanted = n; sum = count = 0; request = true; completed = false;
    return true;
}
void vf_adc_service(void)
{
    if (!ready || completed) return;
    uint32_t now = board_millis();
    if (!waiting) {
        if (!board_adc_begin(measuring_vref)) {
            ready = valid = false; voltage = NAN; return;
        }
        waiting = true; conversion_ms = now; return;
    }
    uint16_t raw;
    if (!board_adc_sample(&raw)) {
        if ((uint32_t)(now-conversion_ms) > 5U) {
            board_adc_cancel(); ready = valid = waiting = false; voltage = NAN;
        }
        return;
    }
    waiting = false;
    if (measuring_vref) {
        float estimate = vdda_from_vrefint(reference,raw);
        fallback = !isfinite(estimate); vdda = fallback ? VDDA_NOMINAL_V : estimate;
        measuring_vref = false; vref_ms = now; return;
    }
    g_vf_debug.last_vf_raw=raw;
    if (raw == 0 || raw >= 4095) {
        valid = false; voltage = NAN; sum = count = 0;
        if (request) completed = true;
        return;
    }
    sum += raw;
    if (++count < wanted) return;
    voltage = convert((float)sum/count); valid = isfinite(voltage);
    sample_ms = now; sum = count = 0;
    if (request) completed = true;
    else measuring_vref = (uint32_t)(now-vref_ms) >= 250U;
}
bool vf_adc_take_samples(float *out)
{
    if (!out || !request || !completed) return false;
    *out = valid ? voltage : NAN;
    request = completed = false; wanted = THERMAL_ADC_AVERAGE_COUNT;
    return true; /* Completion may carry NAN; job must reject it. */
}
bool vf_adc_read_voltage(float *out)
{
    if (!out) return false;
    *out = NAN;
    if (!ready || !valid || (uint32_t)(board_millis()-sample_ms) > 1000U) return false;
    *out = voltage; return true;
}
bool vf_adc_capture(unsigned n, float *out)
{
    g_vf_debug.capture_attempts++;g_vf_debug.requested_samples=n;
    g_vf_debug.last_voltage=NAN;
    g_vf_debug.reference_age_ms=(uint32_t)(board_millis()-vref_ms);
    if (!out) {g_vf_debug.capture_status=1;return false;}
    *out=NAN;
    if (!ready) {g_vf_debug.capture_status=2;return false;}
    if (request) {g_vf_debug.capture_status=3;return false;}
    if (g_vf_debug.reference_age_ms>1000U) {g_vf_debug.capture_status=4;return false;}
    board_adc_cancel();waiting=false;sum=count=0;
    float raw;
    if (!board_adc_burst(n,&raw)) {g_vf_debug.capture_status=5;valid=false;voltage=NAN;return false;}
    voltage=convert(raw);valid=isfinite(voltage);sample_ms=board_millis();
    g_vf_debug.capture_status=valid?0U:6U;g_vf_debug.last_voltage=voltage;
    *out=voltage;return valid;
}
