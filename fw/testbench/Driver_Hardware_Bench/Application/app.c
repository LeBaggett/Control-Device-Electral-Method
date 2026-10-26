#include "app.h"
#include "hw_debug.h"
#include "bench_config.h"
#include "board.h"
#include "bench_led.h"
#include "dwt_us.h"
#include "oled.h"
#include "encoder.h"
#include "max31855.h"
#include "vf_adc.h"
#include "sdlog.h"
#include "sdprobe.h"
#include "sd_io.h"
#include "format.h"
#include <math.h>
#include <stdio.h>
volatile bench_status_t g_bench_status;
volatile bench_control_t g_bench_control;
static uint32_t measured_ms,drawn_ms;
static unsigned action;
static bool data_page,tc_valid;
static void safe(void) {
 board_force_safe();g_bench_status.relay_on=0;g_bench_status.select_heating=0;
}
static bench_control_result_t command(bench_command_t cmd) {
 if(cmd==BENCH_CMD_SAFE) {safe();return BENCH_CONTROL_OK;}
 if(cmd==BENCH_CMD_RELAY_OFF) {board_power_relay_off();g_bench_status.relay_on=0;return BENCH_CONTROL_OK;}
 if(cmd==BENCH_CMD_LED_ON || cmd==BENCH_CMD_LED_OFF) {
  g_bench_status.led_on=cmd==BENCH_CMD_LED_ON;
  board_bench_led_write(g_bench_status.led_on!=0);return BENCH_CONTROL_OK;
 }
 if(cmd==BENCH_CMD_SELECT_MEAS) {
  board_power_select_measurement();g_bench_status.select_heating=0;return BENCH_CONTROL_OK;
 }
 if(cmd==BENCH_CMD_SD_PROBE) {
  if(!BENCH_TEST_SD)return BENCH_CONTROL_SD;
  bool raw_ok=board_sd_read_diagnostic();
  if(raw_ok)sdlog_init(); /* Never mount after incomplete/invalid raw reads. */
  g_bench_status.sd_mount=raw_ok && sdlog_ready()?BENCH_OK:BENCH_FAIL;
  bool ok=raw_ok && sdlog_ready();
  g_bench_status.sd_rw=BENCH_SKIP; /* Diagnostic never writes a test file. */
  return ok?BENCH_CONTROL_OK:BENCH_CONTROL_SD;
 }
 if(cmd!=BENCH_CMD_RELAY_ON && cmd!=BENCH_CMD_SELECT_HEAT)return BENCH_CONTROL_UNKNOWN;
 if(!board_power_control_enabled())return BENCH_CONTROL_LOCKED;
 if(cmd==BENCH_CMD_RELAY_ON) {
  if(g_bench_status.relay_on)return BENCH_CONTROL_OK;
  if(!board_power_relay_on())return BENCH_CONTROL_LOCKED;
  g_bench_status.relay_on=1;return BENCH_CONTROL_OK;
 }
 if(!board_power_select_heating())return BENCH_CONTROL_LOCKED;
 g_bench_status.select_heating=1;return BENCH_CONTROL_OK;
}
static const char *status_text(void) {
 switch(g_bench_control.result) {
 case BENCH_CONTROL_OK:return "COMANDO OK";
 case BENCH_CONTROL_LOCKED:return "POTENCIA BLOQUEADA";
 case BENCH_CONTROL_SD:return "FALHA SD";
 case BENCH_CONTROL_CANCELLED:return "PARADA MANUAL";
 default:return "COMANDO INVALIDO";
 }
}
static void draw(void) {
 if(!BENCH_TEST_OLED || !oled_ready() || oled_busy())return;
 char line[32],temp[16],vf[16];oled_clear();
 oled_text(0,0,data_page?"DADOS DA BANCADA":"BANCADA MANUAL");oled_rect(0,9,128,1,true);
 if(!format_decimal(temp,sizeof(temp),g_bench_status.temperature_c,1))(void)snprintf(temp,sizeof(temp),"--");
 if(!format_decimal(vf,sizeof(vf),g_bench_status.vf_v,3))(void)snprintf(vf,sizeof(vf),"--");
 (void)snprintf(line,sizeof(line),"T:%.6s VF:%.5s",temp,vf);oled_text(0,13,line);
 if(!data_page) {
  (void)snprintf(line,sizeof(line),"RELE:%s SEL:%s",g_bench_status.relay_on?"ON":"OFF",g_bench_status.select_heating?"HEAT":"MED");oled_text(0,23,line);
  const char *name=action==0?(g_bench_status.relay_on?"RELE: DESLIGAR":"RELE: LIGAR"):
   action==1?(g_bench_status.select_heating?"SELECT: MEDICAO":"SELECT: AQUECER"):
   action==2?"DESLIGAR TUDO":action==3?"SD DIAG LEITURA":action==4?(g_bench_status.led_on?"LED: APAGAR":"LED: ACENDER"):"VER DADOS";
  oled_text(0,34,">");oled_text(8,34,name);oled_text(0,44,status_text());
 } else {
  (void)snprintf(line,sizeof(line),"ADC RAW:%u",g_bench_status.adc_raw);oled_text(0,23,line);
  if(!format_decimal(vf,sizeof(vf),g_bench_status.vdda_v,3))(void)snprintf(vf,sizeof(vf),"--");
  (void)snprintf(line,sizeof(line),"VDDA:%s SD:%s",vf,g_bench_status.sd_mount==BENCH_OK?"OK":"OFF");oled_text(0,33,line);
  (void)snprintf(line,sizeof(line),"PD14:%u PD12:%u",g_hw_debug.relay_level,g_hw_debug.select_level);oled_text(0,44,line);
 }
 oled_text(0,55,data_page?"CLIQUE: MENU LONG:OFF":"CLIQUE:EXEC LONG:OFF");(void)oled_present();
}
void app_init(void) {
 g_bench_status=(bench_status_t){0};g_bench_control=(bench_control_t){0};
 action=0;data_page=false;tc_valid=false;measured_ms=drawn_ms=board_millis();
 g_bench_status.temperature_c=g_bench_status.vf_v=g_bench_status.vdda_v=NAN;
 safe();
 g_bench_status.led_configured=board_bench_led_init();
 board_bench_led_write(false);
 g_bench_status.board=board_is_ready()?BENCH_OK:BENCH_FAIL;
 g_bench_status.core_clock_hz=board_bench_core_clock();
 g_bench_status.dwt=board_is_ready() && DWT_US_Delay(1)?BENCH_OK:BENCH_FAIL;
 if(!board_is_ready())return;
 if(BENCH_TEST_OLED) {
  oled_init();
  for(unsigned i=0;i<80U && oled_busy();i++)oled_service();
  draw();
  for(unsigned i=0;i<80U && oled_busy();i++)oled_service();
  oled_service();
  g_bench_status.oled=oled_ready()?BENCH_OK:BENCH_FAIL;
 } else g_bench_status.oled=BENCH_SKIP;
 if(BENCH_TEST_MAX31855)max31855_init();else g_bench_status.thermocouple=BENCH_SKIP;
 if(BENCH_TEST_ADC)vf_adc_init();else g_bench_status.adc=BENCH_SKIP;
 if(BENCH_TEST_ENCODER)encoder_init();else g_bench_status.encoder=BENCH_SKIP;
 g_hw_debug.sd_mount_result=-1;
 g_bench_status.sd_mount=g_bench_status.sd_rw=BENCH_TEST_SD?BENCH_WAIT:BENCH_SKIP;
 safe();
}
void app_run(void) {
 uint32_t now=board_millis();g_bench_status.loop_count++;
 if(!board_is_ready())g_bench_status.board=BENCH_FAIL;
 if(BENCH_TEST_ADC)vf_adc_service();
 if((uint32_t)(now-measured_ms)>=250U) {
  measured_ms=now;float value;
  if(BENCH_TEST_MAX31855) {
   tc_valid=max31855_read(&value)==MAX31855_OK;
   g_bench_status.temperature_c=tc_valid?value:NAN;
   g_bench_status.thermocouple=tc_valid?BENCH_OK:BENCH_FAIL;
  }
  if(BENCH_TEST_ADC) {
   bool ok=vf_adc_read_voltage(&value);g_bench_status.vf_v=ok?value:NAN;
   g_bench_status.adc=ok?BENCH_OK:BENCH_FAIL;g_bench_status.adc_raw=vf_adc_raw_count();
   g_bench_status.vdda_v=vf_adc_vdda();g_bench_status.vdda_estimated=vf_adc_vdda_fallback();
  }
  if(BENCH_TEST_SD && g_hw_debug.sd_attempts>0)g_bench_status.sd_mount=sdlog_ready()?BENCH_OK:BENCH_FAIL;
 }
 input_event_t e=BENCH_TEST_ENCODER?encoder_get_event():INPUT_EVENT_NONE;
 bool emergency=e==INPUT_EVENT_LONG;
 if(e==INPUT_EVENT_CW){g_bench_status.cw++;if(!data_page)action=(action+1U)%6U;}
 if(e==INPUT_EVENT_CCW){g_bench_status.ccw++;if(!data_page)action=(action+5U)%6U;}
 if(emergency){g_bench_status.long_presses++;safe();g_bench_control.result=BENCH_CONTROL_CANCELLED;}
 if(g_bench_control.request_id!=g_bench_control.ack_id) {
  uint32_t request=g_bench_control.request_id;bench_command_t cmd=g_bench_control.command;
  g_bench_control.result=emergency?BENCH_CONTROL_CANCELLED:command(cmd);
  g_bench_control.ack_id=request;
 } else if(e==INPUT_EVENT_CLICK) {
  g_bench_status.clicks++;
  if(data_page)data_page=false;
  else if(action==5)data_page=true;
  else {
   bench_command_t cmd=action==0?(g_bench_status.relay_on?BENCH_CMD_RELAY_OFF:BENCH_CMD_RELAY_ON):
    action==1?(g_bench_status.select_heating?BENCH_CMD_SELECT_MEAS:BENCH_CMD_SELECT_HEAT):
    action==2?BENCH_CMD_SAFE:action==3?BENCH_CMD_SD_PROBE:
    g_bench_status.led_on?BENCH_CMD_LED_OFF:BENCH_CMD_LED_ON;
   g_bench_control.result=command(cmd);
  }
 }
 g_bench_status.action=(uint8_t)action;g_bench_status.page=data_page;
 g_bench_status.encoder=g_bench_status.cw && g_bench_status.ccw && g_bench_status.clicks?BENCH_OK:BENCH_WAIT;
 if(BENCH_TEST_OLED) {
  oled_recover_service();
  g_bench_status.oled=oled_ready()?BENCH_OK:BENCH_FAIL;
  if((uint32_t)(now-drawn_ms)>=200U){drawn_ms=now;draw();}
  oled_service();
 }
 g_bench_status.led_code=tc_valid?0:3;

 board_debug_service();
}
