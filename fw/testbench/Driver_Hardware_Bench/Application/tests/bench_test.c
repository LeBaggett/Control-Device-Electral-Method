#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "app.h"
#include "bench_config.h"
#include "board.h"
#include "oled.h"
#include "max31855.h"
#include "input_event.h"
#include "hw_debug.h"
#include "sdlog.h"
static uint32_t now;
static bool ready=true,tc_ok=true,adc_ok=true,sd_ok=true,allow=true;
static float temp=25;
static input_event_t event;
static bool relay,heating;
static unsigned probes,sd_mount_calls;
static char trace[512];static unsigned used;
static void record(char c){assert(used+1<sizeof(trace));trace[used++]=c;trace[used]=0;}
bool board_is_ready(void){return ready;}
bool board_power_control_enabled(void){return allow;}
uint32_t board_millis(void){return now;}
void board_force_safe(void){relay=heating=false;}
void board_power_select_measurement(void){heating=false;record('M');}
void board_power_relay_off(void){relay=false;record('O');}
bool board_power_relay_on(void){record('R');relay=allow;return relay;}
bool board_power_select_heating(void){record('H');heating=true;return true;}
bool board_bench_led_init(void){return true;}
void board_bench_led_write(bool on){(void)on;}
uint32_t board_bench_core_clock(void){return 168000000;}
bool DWT_US_Delay(uint32_t us){assert(us==1);return true;}
void oled_init(void){}
bool oled_ready(void){return true;}
bool oled_busy(void){return false;}
void oled_clear(void){}
void oled_text(int x,int y,const char *t){(void)x;(void)y;(void)t;}
void oled_rect(int x,int y,int w,int h,bool fill){(void)x;(void)y;(void)w;(void)h;(void)fill;}
bool oled_present(void){return true;}
void oled_service(void){}
void oled_recover_service(void){}
void encoder_init(void){}
input_event_t encoder_get_event(void){input_event_t e=event;event=INPUT_EVENT_NONE;return e;}
void max31855_init(void){}
max31855_status_t max31855_read(float *v){*v=temp;return tc_ok?MAX31855_OK:MAX31855_SPI_ERROR;}
void vf_adc_init(void){}
void vf_adc_service(void){}
bool vf_adc_read_voltage(float *v){*v=0.6f;return adc_ok;}
uint16_t vf_adc_raw_count(void){return adc_ok?744:0;}
float vf_adc_vdda(void){return 3.3f;}
bool vf_adc_vdda_fallback(void){return false;}
void sdlog_init(void){sd_mount_calls++;}
bool sdlog_ready(void){return sd_ok;}
bool board_sd_prepare(void){return sd_ok;}
bool sdprobe_roundtrip(void){probes++;return sd_ok;}
void board_debug_service(void){}
static void step(input_event_t e){now+=250;event=e;app_run();}
static void send(bench_command_t cmd){g_bench_control.command=cmd;g_bench_control.request_id++;step(INPUT_EVENT_NONE);assert(g_bench_control.request_id==g_bench_control.ack_id);}
int main(void){
 tc_ok=false;adc_ok=false;app_init();assert(!relay && !heating && probes==0 && sd_mount_calls==0);
 send(BENCH_CMD_SELECT_HEAT);assert(heating && !relay && g_bench_control.result==BENCH_CONTROL_OK);
 send(BENCH_CMD_RELAY_ON);assert(relay && heating);
 step(INPUT_EVENT_NONE);assert(relay && heating && isnan(g_bench_status.temperature_c) && isnan(g_bench_status.vf_v));
 tc_ok=true;temp=200;step(INPUT_EVENT_NONE);assert(relay && heating);
 tc_ok=false;step(INPUT_EVENT_NONE);assert(relay && heating);
 send(BENCH_CMD_SD_PROBE);assert(relay && heating && probes==1 && sd_mount_calls==1);
 send(BENCH_CMD_SELECT_MEAS);assert(relay && !heating);
 send(BENCH_CMD_SELECT_HEAT);send(BENCH_CMD_RELAY_OFF);assert(!relay && heating);
 send(BENCH_CMD_SAFE);assert(!relay && !heating);
 step(INPUT_EVENT_CLICK);assert(relay && !heating);
 step(INPUT_EVENT_CW);step(INPUT_EVENT_CLICK);assert(relay && heating);
 g_bench_control.command=BENCH_CMD_RELAY_ON;g_bench_control.request_id++;
 step(INPUT_EVENT_LONG);assert(!relay && !heating && g_bench_control.result==BENCH_CONTROL_CANCELLED);
 assert(g_bench_control.ack_id==g_bench_control.request_id);
 send(BENCH_CMD_LED_ON);assert(g_bench_status.led_on==1);
 step(INPUT_EVENT_NONE);assert(g_bench_status.led_on==1);
 send(BENCH_CMD_LED_OFF);assert(g_bench_status.led_on==0);
 allow=false;send(BENCH_CMD_RELAY_ON);assert(!relay && g_bench_control.result==BENCH_CONTROL_LOCKED);
 allow=true;send(BENCH_CMD_RELAY_ON);ready=false;step(INPUT_EVENT_NONE);assert(relay);
 send(BENCH_CMD_RELAY_OFF);assert(!relay);
 puts("Manual-only bench: independent IOs, no sensor/ADC/temperature/SD decisions, manual stop and LED PASS");return 0;
}
