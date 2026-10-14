#include "ui.h"
#include "oled.h"
#include "assets.h"
#include "board.h"
#include "runtime_config.h"
#include "format.h"
#include "config.h"
#include "logo.h"
#include <stdio.h>
static ui_screen_t screen;
static device_mode_t last_mode;
static unsigned selected;
static bool editing;
static uint32_t splash_ms;
static float start_c, end_c, step_c;
static bool is_calibration;
static bool notice_nocal;
ui_screen_t ui_current_screen(void) { return screen; }
void ui_init(void)
{
    screen = UI_SCREEN_SPLASH; last_mode = DEVICE_STANDBY;
    selected = 0; editing = false; splash_ms = board_millis();
    notice_nocal = false;
    start_c = THERMAL_CAL_START_C; end_c = THERMAL_CAL_END_C; step_c = THERMAL_CAL_STEP_C;
}
static void go(ui_screen_t next)
{ screen = next; selected = 0; editing = false; notice_nocal = false; }
static unsigned rotate(unsigned current, unsigned count, input_event_t event)
{
    return event == INPUT_EVENT_CW ? (current+1U)%count : (current+count-1U)%count;
}
ui_cmd_t ui_step(const device_t *d, input_event_t event)
{
    ui_cmd_t cmd = {0};
    if (!d) return cmd;
    if (d->mode != last_mode) {
        last_mode = d->mode;
        if (d->mode == DEVICE_RUNNING) go(UI_SCREEN_RUNNING);
        else if (d->mode == DEVICE_RESULT || d->mode == DEVICE_ABORTED) go(UI_SCREEN_RESULT);
        else if (d->mode == DEVICE_ERROR) go(UI_SCREEN_ERROR);
        else go(UI_SCREEN_HOME);
    }
    if (screen == UI_SCREEN_SPLASH) {
        if ((uint32_t)(board_millis()-splash_ms) >= SPLASH_MS || event == INPUT_EVENT_CLICK)
            go(UI_SCREEN_HOME);
        return cmd;
    }
    if (screen == UI_SCREEN_RUNNING) {
        if (event == INPUT_EVENT_LONG) { cmd.type = UI_CMD_ABORT; go(UI_SCREEN_HOME); }
        return cmd;
    }
    if (event == INPUT_EVENT_LONG) {
        cmd.type = UI_CMD_RETURN_HOME; go(UI_SCREEN_HOME); return cmd;
    }
    bool turn = event == INPUT_EVENT_CW || event == INPUT_EVENT_CCW;
    switch (screen) {
    case UI_SCREEN_HOME:
        if (event == INPUT_EVENT_CLICK) go(UI_SCREEN_MENU);
        break;
    case UI_SCREEN_MENU:
        if (turn) selected = rotate(selected, 3, event);
        if (event == INPUT_EVENT_CLICK) {
            if (selected == 2) go(UI_SCREEN_HOME);
            else {
                is_calibration = selected == 1;
                start_c = 25; end_c = is_calibration ? 85 : 100;
                step_c = is_calibration ? 10 : 5;
                if (!is_calibration && REQUIRE_CAL && !d->calibration_valid) {
                    go(UI_SCREEN_ERROR); notice_nocal = true;
                } else go(UI_SCREEN_SETUP);
            }
        }
        break;
    case UI_SCREEN_SETUP:
        if (turn && !editing) selected = rotate(selected, 5, event);
        else if (turn) {
            float *value = selected == 0 ? &start_c : selected == 1 ? &end_c : &step_c;
            float increment = selected == 2 ? 0.5f : 1.0f;
            float next = *value + (event == INPUT_EVENT_CW ? increment : -increment);
            float min = selected == 2 ? 0.5f : THERMAL_UI_MIN_C;
            float max = selected == 2 ? 50 : THERMAL_UI_MAX_C;
            if (next >= min && next <= max) *value = next;
        }
        if (event == INPUT_EVENT_CLICK) {
            if (selected < 3) editing = !editing;
            else if (selected == 3) go(is_calibration ? UI_SCREEN_CONFIRM_CAL : UI_SCREEN_CONFIRM_TEST);
            else go(UI_SCREEN_HOME);
        }
        break;
    case UI_SCREEN_CONFIRM_TEST:
    case UI_SCREEN_CONFIRM_CAL:
        if (turn) selected = rotate(selected, 2, event);
        if (event == INPUT_EVENT_CLICK) {
            if (selected == 1) go(UI_SCREEN_HOME);
            else {
                cmd.type = screen == UI_SCREEN_CONFIRM_TEST ? UI_CMD_START_TEST : UI_CMD_START_CALIBRATION;
                cmd.start_temperature_c = start_c; cmd.end_temperature_c = end_c;
                cmd.step_temperature_c = step_c;
            }
        }
        break;
    case UI_SCREEN_STATUS:
    case UI_SCREEN_RESULT:
    case UI_SCREEN_ERROR:
        if (event == INPUT_EVENT_CLICK) { cmd.type = UI_CMD_RETURN_HOME; go(UI_SCREEN_HOME); }
        break;
    default: break;
    }
    return cmd;
}
static void title(const char *text)
{
    oled_text(0,0,text); oled_rect(0,9,128,1,true);
}
static void number(int y, const char *label, float value, unsigned decimals)
{
    char numeric[18], line[32];
    if (!format_decimal(numeric,sizeof(numeric),value,decimals))
        (void)snprintf(numeric,sizeof(numeric),"--");
    (void)snprintf(line,sizeof(line),"%s%s",label,numeric); oled_text(0,y,line);
}
static void choice(int y, const char *text, unsigned index)
{
    oled_text(0,y,selected == index ? (editing ? "+" : ">") : " ");
    oled_text(8,y,text);
}
void ui_render(const device_t *d)
{
    if (!d || oled_busy()) return;
    oled_clear(); char line[32];
    switch (screen) {
    case UI_SCREEN_SPLASH:
#if SPLASH_CUSTOM_BITMAP
        oled_xbm((OLED_WIDTH-logo_width)/2,(OLED_HEIGHT-logo_height)/2,
                 logo_width,logo_height,logo_bits,sizeof(logo_bits));
#else
        oled_bitmap(56,5,16,16,image_thermometer,sizeof(image_thermometer));
        oled_text(16,27,"THERMAL BOARD"); oled_text(16,39,"STM32F407");
        oled_text(4,54,"POTENCIA BLOQUEADA");
#endif
        break;
    case UI_SCREEN_HOME:
        (void)snprintf(line,sizeof(line),"STANDBY %02lu:%02lu:%02lu",
            (unsigned long)(d->elapsed_seconds/3600U),
            (unsigned long)((d->elapsed_seconds/60U)%60U),
            (unsigned long)(d->elapsed_seconds%60U)); title(line);
        if (!format_decimal(line,sizeof(line),d->temperature_c,1))
            (void)snprintf(line,sizeof(line),"--");
        oled_text_big(0,14,line); oled_text(98,21,"C");
        number(33,d->vdda_fallback?"VF EST: ":"VF V: ",d->vf_voltage,3);
        (void)snprintf(line,sizeof(line),"SD:%s CAL:%s",d->storage_ready?"OK":"OFF",
                       d->calibration_valid?"OK":"NAO"); oled_text(0,43,line);
        oled_text(0,55,"CLIQUE: MENU"); break;
    case UI_SCREEN_MENU:
        title("CARACTERIZACAO");
        choice(14,"ENSAIO",0); oled_bitmap(112,14,8,8,icon_test,8);
        choice(26,"CALIBRACAO",1); oled_bitmap(112,26,8,8,icon_calibration,8);
        choice(38,"VOLTAR",2); oled_bitmap(112,38,8,8,icon_status,8);
        number(53,"T C: ",d->temperature_c,1); break;
    case UI_SCREEN_SETUP:
        title(is_calibration?"CONFIG CALIBRACAO":"CONFIGURAR ENSAIO");
        for (unsigned i = 0; i < 3; ++i) {
            char value[16];
            (void)format_decimal(value,sizeof(value),i==0?start_c:i==1?end_c:step_c,1);
            (void)snprintf(line,sizeof(line),"%s %s C",i==0?"INICIO":i==1?"FIM":"PASSO",value);
            choice(13+(int)i*10,line,i);
        }
        choice(43,"REVISAR",3); choice(54,"VOLTAR",4); break;
    case UI_SCREEN_CONFIRM_TEST:
    case UI_SCREEN_CONFIRM_CAL:
        title(screen == UI_SCREEN_CONFIRM_CAL ? "CALIBRACAO" : "CONFIRMAR ENSAIO");
        oled_text(0,14,"CONFERIR PARAMETROS"); oled_text(0,25,"RELE BLOQUEADO");
        choice(39,"INICIAR",0); choice(52,"CANCELAR",1); break;
    case UI_SCREEN_STATUS:
        title("DIAGNOSTICO"); number(13,"T C: ",d->temperature_c,1);
        number(23,"VF V: ",d->vf_voltage,3);
        (void)snprintf(line,sizeof(line),"SD:%s CAL:%s",d->storage_ready?"OK":"OFF",
                       d->calibration_valid?"OK":"NAO");
        oled_text(0,35,line); oled_text(0,46,"POTENCIA BLOQUEADA");
        oled_text(0,56,"CLIQUE: VOLTAR"); break;
    case UI_SCREEN_RUNNING:
        (void)snprintf(line,sizeof(line),"%s %02lu:%02lu:%02lu",d->calibrating?"CAL":"HEAT",
            (unsigned long)(d->elapsed_seconds/3600U),
            (unsigned long)((d->elapsed_seconds/60U)%60U),
            (unsigned long)(d->elapsed_seconds%60U)); title(line);
        number(13,"T C: ",d->temperature_c,1);
        number(23,"ALVO: ",d->target_temperature_c,1); number(33,"VF: ",d->vf_voltage,3);
        unsigned progress = d->progress_percent > 100 ? 100 : d->progress_percent;
        oled_rect(0,44,128,6,false); oled_rect(1,45,(int)(126U*progress/100U),4,true);
        oled_text(0,55,"SEGURAR: ABORTAR"); break;
    case UI_SCREEN_RESULT:
        title(d->mode==DEVICE_ABORTED?"ABORTADO":"CONCLUIDO");
        if (d->mode==DEVICE_ABORTED) oled_text(0,27,"OPERACAO ABORTADA");
        else if (d->calibrating && d->calibration_valid) {
            number(13,"M: ",d->calibration.m,2); number(23,"T0: ",d->calibration.t0,2);
            number(33,"R2: ",d->calibration.r2,4);
            (void)snprintf(line,sizeof(line),"N:%u %s",d->calibration.n,
                d->calibration.n < CAL_MIN_POINTS_WARN ? "POUCOS PONTOS" : "");
            oled_text(0,44,line);
        } else oled_text(0,27,"REGISTRO FINALIZADO");
        oled_text(0,55,"CLIQUE: MENU"); break;
    case UI_SCREEN_ERROR:
        title("OPERACAO RECUSADA");
        (void)snprintf(line,sizeof(line),"ERRO: %ld",(long)d->error_code);
        oled_text(0,17,line);
        const char *message = notice_nocal ? "REQUER CALIBRACAO" :
            d->error_code == 11 ? "POTENCIA BLOQUEADA" :
            d->error_code == 6 ? "FALHA CARTAO SD" :
            d->error_code == 7 ? "FALHA TERMOPAR" :
            d->error_code == 8 ? "SOBRETEMPERATURA" :
            d->error_code == 9 ? "AJUSTE INVALIDO" :
            d->error_code == 1 ? "PARAMETROS INVALIDOS" :
            d->error_code == 3 ? "TEMPO ESGOTADO" : "FALHA DE OPERACAO";
        oled_text(0,30,message);
        oled_text(0,43,"RELE DESLIGADO"); oled_text(0,55,"CLIQUE: MENU"); break;
    }
    (void)oled_present();
}
