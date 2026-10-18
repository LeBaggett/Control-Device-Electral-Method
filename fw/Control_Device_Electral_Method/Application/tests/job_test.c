#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "job.h"
#include "hw_debug.h"
#include "board.h"
#include "power.h"
#include "vf_adc.h"
#include "sdlog.h"
#include "calibration.h"
#include "config.h"
static uint32_t now;
static bool storage=true, power_allowed=true, sample_ok=true, constant_vf;
static bool pending, relay, measuring;
static int writes, closes, saved, write_fail_after=-1;
static float temperature;
static char trace[4096], last_line[160]; static size_t trace_n;
static cal_params_t calibration;
static void record(char c) { assert(trace_n+1<sizeof(trace)); trace[trace_n++]=c; trace[trace_n]=0; }
uint32_t board_millis(void) { return now; }
bool board_is_ready(void) { return true; }
void board_delay_us(uint32_t us) { assert(us==30); record('D'); }
void power_safe(void) { relay=false; measuring=true; record('S'); }
bool power_enable_measurement(void) { measuring=true; record('M'); return true; }
bool power_relay_on(void) { assert(measuring); record('R'); relay=power_allowed; return relay; }
bool power_select_heating(void) { assert(relay); measuring=false; record('H'); return true; }
bool vf_adc_ready(void) { return true; }
void vf_adc_cancel_samples(void) { pending=false; }
bool vf_adc_begin_samples(unsigned count) { assert(count==64); pending=true; record('A'); return true; }
bool vf_adc_take_samples(float *value)
{
    if (!pending) return false;
    pending=false; *value=sample_ok?(constant_vf?0.6f:(279-temperature)/357):NAN;
    record('V'); return true;
}
bool vf_adc_capture(unsigned count,float *value)
{
    assert(count==(job_kind()==JOB_KIND_CALIBRATION?CAL_VF_SAMPLES:TEST_VF_SAMPLES));
    assert(measuring); record('V');
    *value=sample_ok?(job_kind()==JOB_KIND_CALIBRATION?(constant_vf?0.6f:(279-temperature)/357):0.6f):NAN; return sample_ok;
}
bool sdlog_ready(void) { return storage; }
bool sdlog_open_run(bool cal) { (void)cal; record('O'); return storage; }
bool sdlog_line(const char *line)
{
    record('W'); if(write_fail_after>=0 && writes>=write_fail_after) return false;
    writes++; (void)snprintf(last_line,sizeof(last_line),"%s",line); return true;
}
bool sdlog_close(void) { closes++; record('C'); return storage; }
bool sdlog_save_calibration(const cal_params_t *cal) { calibration=*cal; saved++; record('F'); return true; }
const cal_params_t *calibration_get(void) { return &calibration; }
void calibration_set(const cal_params_t *cal) { calibration=*cal; }
static void reset(void)
{
    now=0; storage=power_allowed=sample_ok=true; constant_vf=pending=relay=false;
    measuring=true; writes=closes=saved=0; write_fail_after=-1; trace_n=0; trace[0]=0;
    calibration=(cal_params_t){0}; job_init();
}
static job_status_t tick(device_t *d,float temp)
{
    now+=100; temperature=temp; d->temperature_c=temp; d->sensor_sequence++;
    return job_step(d);
}
static job_status_t run_follow(device_t *d)
{
    job_status_t status=JOB_STATUS_RUNNING;
    for(unsigned i=0;i<1000 && status==JOB_STATUS_RUNNING;i++)
        status=tick(d,d->target_temperature_c?d->target_temperature_c:30);
    return status;
}
int main(void)
{
    device_t d={0}; reset();
    assert(!job_start_calibration_params(30,30,10) && trace_n==0 && job_error()==JOB_ERROR_INVALID_PARAMS);
    assert(!job_start_test(0,150,0.5f));
    assert(!job_start_calibration_params(30,31,10) && trace_n==0 && job_error()==JOB_ERROR_FIT);
    reset(); storage=false; assert(!job_start_test(30,50,10));
    assert(!strchr(trace,'R') && !relay && job_error()==JOB_ERROR_STORAGE);
    reset(); power_allowed=false; assert(!job_start_test(30,50,10));
    assert(job_error()==JOB_ERROR_POWER_LOCKED && !relay && closes==1);
    reset(); assert(job_start_calibration_params(30,50,10));
    assert(strstr(trace,"OMR") && !strchr(trace,'H'));
    d=(device_t){0}; tick(&d,29.75f); assert(!strchr(trace,'V'));
    tick(&d,30); assert(strstr(trace,"MDV") && writes==0);
    assert(!strchr(trace,'A')); /* Immediate synchronous capture, no soak/request. */
    assert(run_follow(&d)==JOB_STATUS_COMPLETED && writes==3 && saved==1 && closes==1);
    assert(calibration.valid && calibration.n==3 && fabsf(calibration.m+357)<0.2f && !relay);
    reset(); constant_vf=true; assert(job_start_calibration_params(30,50,10));
    d=(device_t){0}; assert(run_follow(&d)==JOB_STATUS_FAILED && saved==0);
    assert(job_error()==JOB_ERROR_FIT && !relay);
    reset(); assert(job_start_test(30,50,10)); d=(device_t){0}; tick(&d,20);
    assert(!strchr(trace,'V'));
    tick(&d,30); tick(&d,30); tick(&d,30);
    assert(strstr(trace,"MDVWH") && strstr(last_line,",NaN,") && writes==1);
    job_abort(); assert(job_step(&d)==JOB_STATUS_ABORTED && !relay && closes==1);
    reset(); calibration=(cal_params_t){-357,279,1,3,true};
    assert(job_start_test(30,50,10)); d=(device_t){0};
    tick(&d,30); tick(&d,30); tick(&d,30);
    assert(strstr(last_line,",64.80,")); job_abort();
    reset(); assert(job_start_test(30,50,10)); d=(device_t){0};
    tick(&d,NAN); assert(!relay);
    for(int i=0;i<10;i++) assert(job_step(&d)==JOB_STATUS_RUNNING); /* Same fault counted once. */
    tick(&d,NAN); assert(tick(&d,NAN)==JOB_STATUS_FAILED && job_error()==JOB_ERROR_SENSOR);
    reset(); assert(job_start_test(30,50,10)); d=(device_t){0};
    assert(tick(&d,161)==JOB_STATUS_FAILED && job_error()==JOB_ERROR_OVERTEMP && !relay);
    reset(); assert(job_start_test(30,50,10)); d=(device_t){0};
    now=TEST_TARGET_TIMEOUT_S*1000U;
    assert(job_step(&d)==JOB_STATUS_FAILED && job_error()==JOB_ERROR_TARGET_TIMEOUT);
    reset(); write_fail_after=0; assert(job_start_test(30,50,10)); d=(device_t){0};
    tick(&d,30);
    assert(tick(&d,30)==JOB_STATUS_FAILED && job_error()==JOB_ERROR_STORAGE && !relay);
    reset(); sample_ok=false; assert(job_start_test(30,50,10)); d=(device_t){0};
    tick(&d,30); assert(tick(&d,30)==JOB_STATUS_FAILED && !relay && !strstr(trace,"VH"));
    reset(); assert(job_start_calibration_params(30,31,0.5f));d=(device_t){0};
    tick(&d,30.25f);assert(strstr(trace,"MDV") && writes==0);
    tick(&d,30.25f);assert(writes==1);
    size_t before=trace_n;assert(job_step(&d)==JOB_STATUS_RUNNING && trace_n==before);
    tick(&d,30.75f);tick(&d,30.75f);assert(writes==2);
    tick(&d,31.25f);assert(tick(&d,31.25f)==JOB_STATUS_COMPLETED && writes==3 && saved==1);
    reset();sample_ok=false;assert(job_start_calibration_params(30,31,0.5f));d=(device_t){0};
    assert(tick(&d,30)==JOB_STATUS_FAILED && job_error()==JOB_ERROR_SAMPLE_FAILED && !relay);
    reset();assert(job_start_test(30,50,10));d=(device_t){0};
    g_hw_debug.tc_valid=true;g_hw_debug.tc_raw_c=161;
    assert(tick(&d,150)==JOB_STATUS_FAILED && job_error()==JOB_ERROR_OVERTEMP && !relay);
    g_hw_debug.tc_valid=false;
    puts("Immediate calibration/test capture, small steps and safe exits: PASS"); return 0;
}
