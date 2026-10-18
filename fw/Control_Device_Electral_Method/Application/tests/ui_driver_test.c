#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "oled.h"
#include "hw_debug.h"
#include "ui.h"
#include "encoder.h"
#include "peripheral_io.h"
#include "config.h"
#include "logo.h"
#include "runtime_config.h"
#ifndef TEST_OLED_CONTROLLER
#define TEST_OLED_CONTROLLER OLED_DRIVER_SSD1306
#endif
bool board_oled_prepare(void) { return true; }
uint8_t board_oled_controller(void) { return TEST_OLED_CONTROLLER; }
uint8_t board_oled_column_offset(void) { return TEST_OLED_CONTROLLER==OLED_DRIVER_SH1106?2:0; }
static uint32_t now, counter;
static bool pressed, bus_ok = true;
static unsigned transactions, pages, bytes;
static uint8_t displayed[1024];
static unsigned destination;
static const char *preview_dir;
static void save_screen(const char *name,device_t *d)
{
    if (!preview_dir) return;
    while(oled_busy()) { oled_service(); }
    ui_render(d);
    char path[512]; (void)snprintf(path,sizeof(path),"%s/%s.bin",preview_dir,name);
    FILE *file=fopen(path,"wb"); assert(file);
    const uint8_t *frame=oled_framebuffer();
    for(unsigned y=0;y<64;y++) for(unsigned byte=0;byte<16;byte++) {
        unsigned value=0;
        for(unsigned bit=0;bit<8;bit++)
            value |= ((frame[(y/8)*128+byte*8+bit]>>(y%8))&1U)<<bit;
        fputc((int)value,file);
    }
    assert(fclose(file)==0);
}
uint32_t board_millis(void) { return now; }
bool board_oled_write(const uint8_t *p,size_t n)
{
    if (!bus_ok) return false;
    transactions++;
    if (n == 4 && p[0] == 0) {
        assert((p[1]&0xF8U)==0xB0 && p[2]==
            board_oled_column_offset() && p[3]==0x10);
        destination=(p[1]&7U)*128U; pages++;
    } else if (n == 17 && p[0] == 0x40) {
        assert(destination+16 <= sizeof(displayed));
        memcpy(displayed+destination,p+1,16); destination+=16; bytes+=16;
    }
    return true;
}
bool board_encoder_start(uint32_t *value) { *value=counter; return true; }
uint32_t board_encoder_counter(void) { return counter; }
bool board_encoder_button_pressed(void) { return pressed; }
static ui_cmd_t event(device_t *d,input_event_t e) { return ui_step(d,e); }
int main(int argc,char **argv)
{
    if(argc>1)preview_dir=argv[1];
    oled_init(); assert(oled_ready() && oled_busy());
    while(oled_busy()) { oled_service(); }
    oled_service();
    assert(pages==8 && bytes==1024);
    device_t d={0}; d.temperature_c=25; ui_init(); now=SPLASH_MS-1;
    event(&d,INPUT_EVENT_NONE); assert(ui_current_screen()==UI_SCREEN_SPLASH);
    ui_render(&d);
    /* Check every logo pixel in native framebuffer against the original XBM. */
    const uint8_t *frame=oled_framebuffer();
    for(unsigned y=0;y<64;y++) for(unsigned x=0;x<128;x++)
        assert(((frame[(y/8)*128+x]>>(y%8))&1U) == ((logo_bits[y*16+x/8]>>(x%8))&1U));
    oled_clear(); /* Active transfer must retain splash snapshot. */
    while(oled_busy()) oled_service();
    assert(memcmp(displayed,frame,1024)!=0);
    now=SPLASH_MS; event(&d,INPUT_EVENT_NONE); assert(ui_current_screen()==UI_SCREEN_HOME);
    save_screen("home",&d);
    event(&d,INPUT_EVENT_CLICK); assert(ui_current_screen()==UI_SCREEN_MENU);
    save_screen("menu",&d);
    event(&d,INPUT_EVENT_CW); event(&d,INPUT_EVENT_CLICK);
    assert(ui_current_screen()==UI_SCREEN_SETUP);
    save_screen("setup",&d);
    for(int i=0;i<3;i++) event(&d,INPUT_EVENT_CW);
    event(&d,INPUT_EVENT_CLICK); assert(ui_current_screen()==UI_SCREEN_CONFIRM_CAL);
    save_screen("confirm",&d);
    ui_cmd_t cmd=event(&d,INPUT_EVENT_CLICK);
    assert(cmd.type==UI_CMD_START_CALIBRATION && cmd.start_temperature_c==25 &&
           cmd.end_temperature_c==85 && cmd.step_temperature_c==10);
    d.mode=DEVICE_RUNNING; event(&d,INPUT_EVENT_NONE);
    d.calibrating=1; d.progress_percent=42;d.target_temperature_c=35;d.vf_voltage=0.6f;
    save_screen("running",&d);
    while(oled_busy()) { oled_service(); }
    assert(event(&d,INPUT_EVENT_LONG).type==UI_CMD_ABORT);
    d.mode=DEVICE_ERROR; event(&d,INPUT_EVENT_NONE);
    d.error_code=11;save_screen("error",&d);
    assert(event(&d,INPUT_EVENT_CLICK).type==UI_CMD_RETURN_HOME);
    d.mode=DEVICE_RESULT; d.calibration_valid=1;d.calibration=(cal_params_t){-357,279,1,3,true};
    event(&d,INPUT_EVENT_NONE);save_screen("result",&d);
    while(oled_busy()) { oled_service(); }
    d.mode=DEVICE_STANDBY;d.tc_gain=1;d.tc_required=4;
    ui_init();event(&d,INPUT_EVENT_CLICK);event(&d,INPUT_EVENT_CLICK);
    event(&d,INPUT_EVENT_CW);event(&d,INPUT_EVENT_CW);event(&d,INPUT_EVENT_CLICK);
    assert(ui_current_screen()==UI_SCREEN_TC_CAL);save_screen("tc_menu",&d);
    event(&d,INPUT_EVENT_CW);cmd=event(&d,INPUT_EVENT_CLICK);
    assert(cmd.type==UI_CMD_TC_BEGIN && cmd.tc_points==4);save_screen("tc_points",&d);
    assert(event(&d,INPUT_EVENT_LONG).type==UI_CMD_TC_CANCEL);
    event(&d,INPUT_EVENT_CLICK);event(&d,INPUT_EVENT_CW);event(&d,INPUT_EVENT_CW);event(&d,INPUT_EVENT_CLICK);
    event(&d,INPUT_EVENT_CW);event(&d,INPUT_EVENT_CW);event(&d,INPUT_EVENT_CW);event(&d,INPUT_EVENT_CLICK);
    assert(ui_current_screen()==UI_SCREEN_TC_FINE);save_screen("tc_fine",&d);
    event(&d,INPUT_EVENT_CLICK);event(&d,INPUT_EVENT_CW);event(&d,INPUT_EVENT_CLICK);
    event(&d,INPUT_EVENT_CW);event(&d,INPUT_EVENT_CW);cmd=event(&d,INPUT_EVENT_CLICK);
    assert(cmd.type==UI_CMD_TC_SAVE && fabsf(cmd.tc_offset-0.1f)<0.001f);
    while(oled_busy()) {oled_service();}
    bus_ok=false; assert(oled_present()); oled_service(); assert(!oled_ready());
    g_hw_tuning.encoder_direction=1;
    counter=UINT32_MAX-1; encoder_init(); counter=2;
    assert(encoder_get_event()==INPUT_EVENT_CW);
    counter=UINT32_MAX-1; assert(encoder_get_event()==INPUT_EVENT_CCW);
    pressed=true; now=3000; assert(encoder_get_event()==INPUT_EVENT_NONE);
    now+=30; assert(encoder_get_event()==INPUT_EVENT_NONE);
    pressed=false; now+=100; assert(encoder_get_event()==INPUT_EVENT_NONE);
    now+=30; assert(encoder_get_event()==INPUT_EVENT_CLICK);
    pressed=true; now+=100; encoder_get_event(); now+=30; encoder_get_event();
    now+=LONG_PRESS_MS; assert(encoder_get_event()==INPUT_EVENT_LONG);
    assert(encoder_get_event()==INPUT_EVENT_NONE);
    pressed=false; encoder_get_event(); now+=30;
    assert(encoder_get_event()==INPUT_EVENT_NONE); /* No click after long press. */
    d=(device_t){0};ui_init();event(&d,INPUT_EVENT_CLICK);event(&d,INPUT_EVENT_CLICK);
    for(unsigned i=0;i<3;i++)event(&d,INPUT_EVENT_CW);
    event(&d,INPUT_EVENT_CLICK);assert(ui_current_screen()==UI_SCREEN_K_MANUAL);
    save_screen("manual_k",&d);
    event(&d,INPUT_EVENT_CW);event(&d,INPUT_EVENT_CW);event(&d,INPUT_EVENT_CLICK);
    assert(ui_current_screen()==UI_SCREEN_K_MANUAL); /* Must explicitly review K. */
    event(&d,INPUT_EVENT_CCW);event(&d,INPUT_EVENT_CCW);event(&d,INPUT_EVENT_CLICK);
    event(&d,INPUT_EVENT_CCW);event(&d,INPUT_EVENT_CLICK); /* -510 C/V, commit. */
    event(&d,INPUT_EVENT_CW);event(&d,INPUT_EVENT_CW);event(&d,INPUT_EVENT_CLICK);
    assert(ui_current_screen()==UI_SCREEN_SETUP);
    for(unsigned i=0;i<3;i++)event(&d,INPUT_EVENT_CW);
    event(&d,INPUT_EVENT_CLICK);save_screen("confirm_manual_k",&d);
    cmd=event(&d,INPUT_EVENT_CLICK);assert(cmd.type==UI_CMD_START_TEST_K&&cmd.manual_k_c_per_v==-510);
    d=(device_t){0};ui_init();event(&d,INPUT_EVENT_CLICK);event(&d,INPUT_EVENT_CLICK);
    event(&d,INPUT_EVENT_CW);event(&d,INPUT_EVENT_CW);event(&d,INPUT_EVENT_CLICK);
    event(&d,INPUT_EVENT_CW);event(&d,INPUT_EVENT_CW);event(&d,INPUT_EVENT_CLICK);
    assert(ui_current_screen()==UI_SCREEN_TC_HEAT_SETUP);save_screen("tc_heat_setup",&d);
    event(&d,INPUT_EVENT_CW);event(&d,INPUT_EVENT_CW);event(&d,INPUT_EVENT_CW);
    cmd=event(&d,INPUT_EVENT_CLICK);assert(cmd.type==UI_CMD_TC_HEAT_BEGIN && cmd.tc_points==3 && cmd.start_temperature_c==30 && cmd.step_temperature_c==10);
    d.tc_heat_active=true;d.tc_heat_target_c=30;d.tc_required=3;save_screen("tc_heating",&d);
    d.tc_heat_stable=true;save_screen("tc_heat_ready",&d);
    d.tc_heat_error=2;save_screen("tc_heat_failure",&d);
    assert(event(&d,INPUT_EVENT_LONG).type==UI_CMD_TC_CANCEL);
    puts("OLED, logo, menus, signed manual K review,  encoder: PASS"); return 0;
}
