#ifndef VF_ADC_H
#define VF_ADC_H
#include <stdbool.h>
#include <stdint.h>

void vf_adc_init(void);
uint16_t vf_adc_raw_count(void);
bool vf_adc_read_voltage(float *voltage);
void vf_adc_service(void);
bool vf_adc_ready(void);
bool vf_adc_begin_samples(unsigned count);
bool vf_adc_take_samples(float *voltage);
bool vf_adc_capture(unsigned count, float *voltage);
float vf_adc_vdda(void);
bool vf_adc_vdda_fallback(void);
void vf_adc_cancel_samples(void);

#endif
