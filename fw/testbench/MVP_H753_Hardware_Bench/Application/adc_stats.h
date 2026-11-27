#ifndef BENCH_ADC_STATS_H
#define BENCH_ADC_STATS_H
#include <stdint.h>
typedef struct {uint32_t mean,minimum,maximum,millivolts;float volts;} adc_stats_t;
adc_stats_t adc_block_stats(const uint16_t samples[32],uint32_t reference_mv);
#endif
