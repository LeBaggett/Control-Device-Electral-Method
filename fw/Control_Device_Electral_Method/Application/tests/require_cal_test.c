#include <assert.h>
#include <stdio.h>
#include "job.h"
#include "calibration.h"
#include "sdlog.h"
#include "vf_adc.h"
#include "power.h"
#include "ui.h"
#include "board.h"
#include "oled.h"
#include "config.h"
static cal_params_t cal;
static unsigned side_effects;
const cal_params_t *calibration_get(void) { return &cal; }
void calibration_set(const cal_params_t *p) { cal=*p; }
bool sdlog_ready(void) { side_effects++;return false; }
bool sdlog_open_run(bool c) { (void)c;side_effects++;return false; }
bool sdlog_line(const char *line) { (void)line;side_effects++;return false; }
bool sdlog_close(void) { side_effects++;return false; }
bool sdlog_save_calibration(const cal_params_t *p) { (void)p;side_effects++;return false; }
bool vf_adc_ready(void) { side_effects++;return false; }
bool vf_adc_begin_samples(unsigned n) { (void)n;return false; }
bool vf_adc_take_samples(float *p) { (void)p;return false; }
bool vf_adc_capture(unsigned n,float *p) { (void)n;(void)p;return false; }
void vf_adc_cancel_samples(void) { side_effects++; }
void power_safe(void) { side_effects++; }
bool power_enable_measurement(void) { side_effects++;return false; }
bool power_relay_on(void) { side_effects++;return false; }
bool power_select_heating(void) { side_effects++;return false; }
uint32_t board_millis(void) { return SPLASH_MS; }
bool board_is_ready(void) { return true; }
void board_delay_us(uint32_t us) { (void)us; }
/* Draw dependencies are inert here; actual renderer is covered separately. */
bool oled_busy(void) { return false; }
void oled_clear(void) { }
void oled_text(int x,int y,const char *s) { (void)x;(void)y;(void)s; }
void oled_text_big(int x,int y,const char *s) { (void)x;(void)y;(void)s; }
void oled_rect(int x,int y,int w,int h,bool f) { (void)x;(void)y;(void)w;(void)h;(void)f; }
void oled_bitmap(int x,int y,unsigned w,unsigned h,const uint8_t *p,size_t n)
{ (void)x;(void)y;(void)w;(void)h;(void)p;(void)n; }
void oled_xbm(int x,int y,unsigned w,unsigned h,const uint8_t *p,size_t n)
{ (void)x;(void)y;(void)w;(void)h;(void)p;(void)n; }
bool oled_present(void) { return false; }
int main(void)
{
    assert(REQUIRE_CAL==1); job_init();
    assert(!job_start_test(25,100,5) && job_error()==JOB_ERROR_NO_CALIBRATION && side_effects==0);
    device_t d={0};ui_init();ui_step(&d,INPUT_EVENT_CLICK); /* Splash -> Home. */
    ui_step(&d,INPUT_EVENT_CLICK); /* Home -> Menu. */
    assert(ui_step(&d,INPUT_EVENT_CLICK).type==UI_CMD_NONE);
    assert(ui_current_screen()==UI_SCREEN_ERROR);
    puts("REQUIRE_CAL: refuses before SD/power and warns in UI: PASS");return 0;
}
