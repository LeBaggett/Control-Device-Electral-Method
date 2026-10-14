#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "targets.h"
#include "linfit.h"
#include "vf_math.h"
#include "format.h"
int main(void)
{
    float v[] = {0.50f,0.60f,0.64f}, t[] = {100.5f,64.8f,50.52f};
    cal_params_t cal;
    assert(linfit(v,t,3,&cal) && fabsf(cal.m+357) < 0.02f && fabsf(cal.t0-279) < 0.02f);
    assert(cal.r2 > 0.9999f && fabsf(cal_tj(&cal,0.60f)-64.8f) < 0.01f);
    assert(!linfit(v,t,1,&cal) && !cal.valid);
    float same[] = {0.5f,0.5f,0.5f}; assert(!linfit(same,t,3,&cal));
    t[1] = NAN; assert(!linfit(v,t,3,&cal));
    sweep_cfg_t c = {30,60,10}; assert(targets_count(&c)==4 && targets_at(&c,3)==60);
    c.t_step_c=7; assert(targets_count(&c)==5 && targets_at(&c,4)==58);
    c.t_step_c=0; assert(targets_count(&c)==0);
    c=(sweep_cfg_t){30,31,10}; assert(targets_count(&c)==1);
    c=(sweep_cfg_t){0,150,0.5f}; assert(targets_count(&c)==0);
    c=(sweep_cfg_t){30,30,10}; assert(targets_count(&c)==0);
    c=(sweep_cfg_t){-1,100,10}; assert(targets_count(&c)==0);
    c=(sweep_cfg_t){30,151,10}; assert(targets_count(&c)==0);
    assert(fabsf(vf_from_counts(2048,3.3f)-1.6504f)<0.0001f);
    assert(vdda_from_vrefint(1500,1500)==3.3f && isnan(vdda_from_vrefint(0,1500)));
    char s[32]; assert(format_decimal(s,sizeof(s),-357.0f,3) && strcmp(s,"-357.000")==0);
    assert(!format_decimal(s,sizeof(s),NAN,2));
    assert(thermal_crc32((const uint8_t*)"123456789",9)==0xCBF43926U);
    puts("targets, linear fit, Vf and formatting: PASS");
    return 0;
}
