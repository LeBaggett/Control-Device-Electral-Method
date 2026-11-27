#ifndef BENCH_ADC_SERVICE_H
#define BENCH_ADC_SERVICE_H
#include <stdint.h>
#include <stddef.h>
typedef struct {
 uint32_t phase,init_status,error,valid,blocks,skipped_blocks,last_block_ms;
 uint32_t mean_raw,min_raw,max_raw,last_raw,millivolts;
 float input_voltage;
} adc_debug_t;
extern volatile adc_debug_t g_adc_debug;
void adc_init(void);
void adc_step(uint32_t now);
int adc_json(char *out,size_t capacity);
#endif
