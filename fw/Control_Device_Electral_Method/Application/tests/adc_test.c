#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "vf_adc.h"
#include "peripheral_io.h"
static uint32_t now;
static unsigned starts;
static uint16_t vf=744, reference=1500;
static bool vref, available=true;
uint32_t board_millis(void) { return now; }
bool board_adc_start(void) { return true; }
bool board_adc_begin(bool internal) { vref=internal; starts++; return true; }
bool board_adc_sample(uint16_t *value) { if(!available)return false; *value=vref?reference:vf; return true; }
void board_adc_cancel(void) { }
uint16_t board_vrefint_calibration(void) { return 1500; }
bool board_adc_burst(unsigned count,float *avg) { assert(count==8); *avg=vf; return true; }
static void service(unsigned times) { for(unsigned i=0;i<times;i++){vf_adc_service();now++;} }
int main(void)
{
    vf_adc_init(); assert(vf_adc_ready()); float voltage;
    assert(!vf_adc_read_voltage(&voltage) && isnan(voltage));
    service(40); assert(vf_adc_read_voltage(&voltage) && fabsf(voltage-0.59956f)<0.001f);
    assert(fabsf(vf_adc_vdda()-3.3f)<0.001f && !vf_adc_vdda_fallback());
    assert(vf_adc_begin_samples(64)); service(130);
    assert(vf_adc_take_samples(&voltage) && isfinite(voltage));
    assert(!vf_adc_take_samples(&voltage));
    assert(vf_adc_capture(8,&voltage) && isfinite(voltage));
    vf=4095; service(2); assert(!vf_adc_read_voltage(&voltage));
    vf=744; reference=0; assert(vf_adc_begin_samples(4)); service(10);
    assert(vf_adc_take_samples(&voltage) && vf_adc_vdda_fallback());
    vf_adc_cancel_samples(); available=false; service(8);
    assert(!vf_adc_ready()); assert(starts>0);
    puts("ADC/VREFINT average, fallback and failure: PASS"); return 0;
}
