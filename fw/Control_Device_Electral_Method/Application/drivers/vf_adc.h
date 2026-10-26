#ifndef VF_ADC_H
#define VF_ADC_H
#include <stdbool.h>
#include <stdint.h>
typedef struct {
 uint32_t capture_attempts,capture_status,burst_status,requested_samples,completed_samples;
 uint32_t last_vf_raw,last_burst_raw,elapsed_cycles,reference_age_ms;
 uint32_t reference_refreshes,reference_status;
 float last_voltage;
} vf_adc_debug_t;
extern volatile vf_adc_debug_t g_vf_debug;

void vf_adc_init(void);
bool vf_adc_read_voltage(float *voltage);
void vf_adc_service(void);
bool vf_adc_ready(void);
/* Blocking, bounded VREFINT refresh. Use outside the hot acquisition window. */
bool vf_adc_refresh_reference(void);
bool vf_adc_begin_samples(unsigned count);
bool vf_adc_take_samples(float *voltage);
bool vf_adc_capture(unsigned count, float *voltage);
float vf_adc_vdda(void);
bool vf_adc_vdda_fallback(void);
void vf_adc_cancel_samples(void);

#endif
