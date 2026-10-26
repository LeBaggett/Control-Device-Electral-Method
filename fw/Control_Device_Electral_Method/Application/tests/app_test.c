#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "app.h"
#include "hw_debug.h"
#include "board.h"
#include "peripheral_io.h"
#include "ui.h"
#include "config.h"
#include "job.h"
static uint32_t now, counter;
static bool button, sensor_ok=true, vref;
static unsigned on_attempts, sd_retries;
void board_init(void) { }
bool board_is_ready(void) { return true; }
bool board_power_control_enabled(void) { return false; }
void board_force_safe(void) { }
uint32_t board_millis(void) { return now; }
void board_delay_us(uint32_t us) { (void)us; }
void board_power_select_measurement(void) { }
void board_power_relay_off(void) { }
bool board_power_relay_on(void) { on_attempts++; return false; }
bool board_power_select_heating(void) { assert(0); return false; }
bool board_max31855_read32(uint32_t *raw) { *raw=100U<<18; return sensor_ok; }
bool board_oled_prepare(void) { return true; }
uint8_t board_oled_controller(void) { return OLED_DRIVER_SSD1306; }
uint8_t board_oled_column_offset(void) { return 0; }
bool board_oled_write(const uint8_t *data,size_t size) { (void)data;(void)size;return true; }
bool board_encoder_start(uint32_t *value) { *value=counter;return true; }
uint32_t board_encoder_counter(void) { return counter; }
bool board_encoder_button_pressed(void) { return button; }
bool board_adc_start(void) { return true; }
bool board_adc_begin(bool ref) { vref=ref;return true; }
void board_adc_cancel(void) { }
bool board_adc_sample(uint16_t *raw) { *raw=vref?1500:744;return true; }
uint16_t board_vrefint_calibration(void) { return 1500; }
bool board_adc_burst(unsigned n,float *average) { (void)n;*average=744;return true; }
static void run(unsigned milliseconds)
{ for(unsigned i=0;i<milliseconds;i++){now++;app_run();} }
static void click(void)
{ button=true;run(40);button=false;run(40); }
int main(void)
{
    board_init();app_init();run(SPLASH_MS);
    const device_t *d=app_device();
    assert(ui_current_screen()==UI_SCREEN_HOME && fabsf(d->temperature_c-25)<0.001f);
    assert(isfinite(d->vf_voltage) && !d->storage_ready && !d->calibration_valid);
    click();assert(ui_current_screen()==UI_SCREEN_MENU);
    click();assert(ui_current_screen()==UI_SCREEN_SETUP);
    for(int i=0;i<3;i++){counter+=(uint32_t)((int32_t)g_hw_tuning.encoder_counts_per_detent*g_hw_tuning.encoder_direction);run(1);}
    click();assert(ui_current_screen()==UI_SCREEN_CONFIRM_TEST);
    click();assert(d->mode==DEVICE_ERROR && d->error_code==JOB_ERROR_STORAGE);
    assert(on_attempts==0 && ui_current_screen()==UI_SCREEN_ERROR);
    click();assert(d->mode==DEVICE_STANDBY && ui_current_screen()==UI_SCREEN_HOME);
    sensor_ok=false;run(250);assert(isnan(d->temperature_c));
    button=true;run(LONG_PRESS_MS+50);button=false;run(40);
    assert(sd_retries==1 && on_attempts==0);
    puts("full application links; no-SD start and sensor failure: PASS");return 0;
}

void board_debug_service(void) { }

bool board_sd_prepare(void) { sd_retries++; return false; }

#include "sofia_io.h"
bool sofia_io_capture(unsigned delay,sofia_capture_t *out){(void)delay;(void)out;return false;}
uint32_t sofia_io_elapsed_us(uint32_t start){(void)start;return 0;}
