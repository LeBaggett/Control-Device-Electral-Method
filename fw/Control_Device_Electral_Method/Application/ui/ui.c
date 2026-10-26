#include "ui.h"
#include "thermal_tsp.h"
#include "hw_debug.h"
#include "oled.h"
#include "assets.h"
#include "board.h"
#include "runtime_config.h"
#include "format.h"
#include "config.h"
#include "logo.h"
#include <stdio.h>
#include <math.h>
static ui_screen_t screen;
static device_mode_t last_mode;
static unsigned selected;
static bool editing;
static uint32_t splash_ms;
static float start_c, end_c, step_c;
static sofia_config_t sofia_cfg;
static bool is_calibration, manual_k_mode, k_reviewed, k_notice;
static float manual_k=-500.0f;
static unsigned k_step_index;
static const float k_steps[]={1,10,100,1000};
static bool notice_nocal, tc_notice;
static float tc_reference, tc_manual_offset, tc_alpha;
static unsigned tc_seen_points,tc_heat_points;
static float tc_heat_first,tc_heat_step;
static bool tc_auto,tc_ref_reviewed,tc_ref_notice;
ui_screen_t ui_current_screen(void) { return screen; }
void ui_init(void)
{
    screen = UI_SCREEN_SPLASH; last_mode = DEVICE_STANDBY;
    selected = 0; editing = false; splash_ms = board_millis();
    notice_nocal = false; manual_k_mode=false;manual_k=-500;k_step_index=1;k_reviewed=false;k_notice=false;
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
        else if(event==INPUT_EVENT_CLICK && d->sofia_active)cmd.type=UI_CMD_SOFIA_MEASURE;
        return cmd;
    }
    if (event == INPUT_EVENT_LONG) {
        cmd.type = (screen==UI_SCREEN_TC_CAL || screen==UI_SCREEN_TC_MULTI || screen==UI_SCREEN_TC_FINE || screen==UI_SCREEN_TC_RESET || screen==UI_SCREEN_TC_HEAT_SETUP) ? UI_CMD_TC_CANCEL : UI_CMD_RETURN_HOME; go(UI_SCREEN_HOME); return cmd;
    }
    bool turn = event == INPUT_EVENT_CW || event == INPUT_EVENT_CCW;
    switch (screen) {
    case UI_SCREEN_HOME:
        if (event == INPUT_EVENT_CLICK) go(UI_SCREEN_MENU);
        break;
    case UI_SCREEN_MENU:
        if (turn) selected = rotate(selected, 6, event);
        if (event == INPUT_EVENT_CLICK) {
            if (selected == 5) go(UI_SCREEN_HOME);
            else if(selected==4) {
                sofia_cfg=(sofia_config_t){d->calibration_valid?d->calibration.m:-467.0f,
                    d->calibration_valid?d->calibration.t0:279.36f,85,1,20};
                go(UI_SCREEN_SOFIA_SETUP);
            }
            else if(selected == 3) {
                k_notice=false;manual_k_mode=false;go(UI_SCREEN_K_MANUAL);
            }
            else if (selected == 2) {
                tc_reference=isfinite(d->temperature_c)?roundf(d->temperature_c*10.0f)/10.0f:25.0f;
                if (tc_reference<TEMP_MIN_C || tc_reference>TEMP_MAX_C) tc_reference=25;
                tc_notice=false;go(UI_SCREEN_TC_CAL);
            } else {
                is_calibration = selected == 1;manual_k_mode=false;
                start_c = 25; end_c = is_calibration ? 85 : 100;
                step_c = is_calibration ? 10 : 5;
                if (!is_calibration && REQUIRE_CAL && !d->calibration_valid) {
                    go(UI_SCREEN_ERROR); notice_nocal = true;
                } else go(UI_SCREEN_SETUP);
            }
        }
        break;
    case UI_SCREEN_SOFIA_SETUP:
        if(turn && !editing)selected=rotate(selected,7,event);
        else if(turn) {
            float sign=event==INPUT_EVENT_CW?1.0f:-1.0f;
            if(selected==0){int n=(int)sofia_cfg.heat_s+(int)sign;if(n>=1 && n<=600)sofia_cfg.heat_s=(unsigned)n;}
            if(selected==1){float n=sofia_cfg.slope+sign*.1f;if(n>=-100000 && n< -1)sofia_cfg.slope=n;}
            if(selected==2){float n=sofia_cfg.intercept+sign*.01f;if(n>=-10000 && n<=10000)sofia_cfg.intercept=n;}
            if(selected==3){int n=(int)sofia_cfg.delay_us+(int)sign;if(n>=5 && n<=1000)sofia_cfg.delay_us=(unsigned)n;}
            if(selected==4){float n=sofia_cfg.max_case_c+sign;if(n>=30 && n<=120)sofia_cfg.max_case_c=n;}
        }
        if(event==INPUT_EVENT_CLICK) {
            if(selected<5)editing=!editing;
            else if(selected==5)go(UI_SCREEN_SOFIA_CONFIRM);
            else go(UI_SCREEN_MENU);
        }
        break;
    case UI_SCREEN_SOFIA_CONFIRM:
        if(turn)selected=rotate(selected,2,event);
        if(event==INPUT_EVENT_CLICK) {
            if(selected==1)go(UI_SCREEN_SOFIA_SETUP);
            else {cmd.type=UI_CMD_START_SOFIA;cmd.sofia=sofia_cfg;}
        }
        break;
    case UI_SCREEN_K_MANUAL:
        if(turn && editing) {
            float next=manual_k+(event==INPUT_EVENT_CW?1.0f:-1.0f)*k_steps[k_step_index];
            if(next>=-100000.0f && next<=100000.0f){manual_k=next;k_reviewed=false;k_notice=false;}
        } else if(turn) selected=rotate(selected,4,event);
        if(event==INPUT_EVENT_CLICK) {
            if(selected==0) {
                editing=!editing;
                if(!editing)k_reviewed=tsp_slope_valid(manual_k);
            } else if(selected==1)k_step_index=(k_step_index+1U)%4U;
            else if(selected==2) {
                if(!k_reviewed || !tsp_slope_valid(manual_k)){k_notice=true;break;}
                manual_k_mode=true;is_calibration=false;
                start_c=25;end_c=100;step_c=5;go(UI_SCREEN_SETUP);
            } else go(UI_SCREEN_MENU);
        }
        break;
    case UI_SCREEN_TC_CAL:
        if(turn) selected=rotate(selected,6,event);
        if(event==INPUT_EVENT_CLICK) {
            if(selected<2) {
                tc_auto=false;cmd.type=UI_CMD_TC_BEGIN;cmd.tc_points=selected==0?3U:4U;
                tc_seen_points=0;tc_reference=25;go(UI_SCREEN_TC_MULTI);
            } else if(selected==2) {
                tc_heat_points=3;tc_heat_first=30;tc_heat_step=10;tc_auto=true;
                go(UI_SCREEN_TC_HEAT_SETUP);
            } else if(selected==3) {
                tc_manual_offset=d->tc_offset_c;tc_alpha=g_hw_tuning.display_filter_alpha;tc_notice=false;
                go(UI_SCREEN_TC_FINE);
            } else if(selected==4) {tc_notice=false;go(UI_SCREEN_TC_RESET);}
            else {cmd.type=UI_CMD_TC_CANCEL;go(UI_SCREEN_MENU);}
        }
        break;
    case UI_SCREEN_TC_HEAT_SETUP:
        if(turn && editing) {
            if(selected==0)tc_heat_points=tc_heat_points==3?4:3;
            else {
                float *v=selected==1?&tc_heat_first:&tc_heat_step;
                float next=*v+(event==INPUT_EVENT_CW?1.0f:-1.0f);
                if(next>=(selected==1?TC_HEAT_MIN_C:TC_HEAT_MIN_STEP_C) && next<=TC_HEAT_MAX_C)*v=next;
            }
        } else if(turn)selected=rotate(selected,5,event);
        if(event==INPUT_EVENT_CLICK) {
            if(selected<3)editing=!editing;
            else if(selected==3) {
                cmd.type=UI_CMD_TC_HEAT_BEGIN;cmd.tc_points=tc_heat_points;
                cmd.start_temperature_c=tc_heat_first;cmd.step_temperature_c=tc_heat_step;
                tc_seen_points=0;tc_reference=tc_heat_first;tc_ref_reviewed=false;tc_ref_notice=false;go(UI_SCREEN_TC_MULTI);selected=1;
            } else {tc_auto=false;go(UI_SCREEN_TC_CAL);}
        }
        break;
    case UI_SCREEN_TC_MULTI:
        if(d->tc_capture_busy)break;
        if(d->tc_points!=tc_seen_points) {
            tc_seen_points=d->tc_points;tc_ref_reviewed=false;tc_ref_notice=false;
            if(d->tc_points<d->tc_required && tc_reference+(tc_auto?tc_heat_step:10)<=TEMP_MAX_C)tc_reference+=tc_auto?tc_heat_step:10;
        }
        if(tc_auto) {
            if(turn)selected=selected==1?2:1;
            if(event==INPUT_EVENT_CLICK) {
                if(selected==1)cmd.type=d->tc_heat_waiting_next?UI_CMD_TC_HEAT_NEXT:UI_CMD_TC_CAPTURE;
                else cmd.type=UI_CMD_TC_APPLY_FIT;
            }
            break;
        }
        if(turn && editing) {
            float next=tc_reference+(event==INPUT_EVENT_CW?0.1f:-0.1f);
            if(next>=TEMP_MIN_C && next<=TEMP_MAX_C)tc_reference=next;
        } else if(turn)selected=rotate(selected,3,event);
        if(event==INPUT_EVENT_CLICK) {
            if(selected==0){editing=!editing;if(!editing){tc_ref_reviewed=true;tc_ref_notice=false;}}
            else if(selected==1) {
                if(tc_auto && d->tc_heat_active && d->tc_heat_waiting_next)cmd.type=UI_CMD_TC_HEAT_NEXT;
                else {if(tc_auto && !tc_ref_reviewed){tc_ref_notice=true;break;}cmd.type=UI_CMD_TC_CAPTURE;cmd.reference_temperature_c=tc_reference;}
            }
            else cmd.type=UI_CMD_TC_APPLY_FIT;
        }
        break;
    case UI_SCREEN_TC_FINE:
        if(turn && editing) {
            float increment=event==INPUT_EVENT_CW?1.0f:-1.0f;
            if(selected==0) {
                float next=tc_manual_offset+increment*0.1f;
                if(next>=-20 && next<=20)tc_manual_offset=next;
            } else {
                float next=tc_alpha+increment*0.05f;
                if(next>=0.049f && next<=1.001f)tc_alpha=fminf(1,fmaxf(0.05f,next));
            }
            tc_notice=false;
        } else if(turn) selected=rotate(selected,4,event);
        if(event==INPUT_EVENT_CLICK) {
            if(selected<2)editing=!editing;
            else {
                if(selected==3) {tc_notice=false;go(UI_SCREEN_TC_RESET);}
                else {cmd.type=UI_CMD_TC_SAVE;cmd.tc_offset=tc_manual_offset;cmd.tc_alpha=tc_alpha;tc_notice=true;}
            }
        }
        break;
    case UI_SCREEN_TC_RESET:
        if(turn)selected=rotate(selected,2,event);
        if(event==INPUT_EVENT_CLICK) {
            if(selected==0) {cmd.type=UI_CMD_TC_RESET;tc_notice=true;tc_seen_points=0;tc_reference=25;}
            else {cmd.type=UI_CMD_TC_CANCEL;tc_notice=false;go(UI_SCREEN_TC_CAL);}
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
                cmd.type = screen == UI_SCREEN_CONFIRM_TEST ?
                    (manual_k_mode?UI_CMD_START_TEST_K:UI_CMD_START_TEST):
                    UI_CMD_START_CALIBRATION;
                cmd.manual_k_c_per_v=manual_k;
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
        oled_text(4,54,d->power_control_enabled?"POTENCIA HABILITADA":"POTENCIA BLOQUEADA");
#endif
        break;
    case UI_SCREEN_HOME:
        (void)snprintf(line,sizeof(line),"STANDBY %02lu:%02lu:%02lu",
            (unsigned long)(d->elapsed_seconds/3600U),
            (unsigned long)((d->elapsed_seconds/60U)%60U),
            (unsigned long)(d->elapsed_seconds%60U)); title(line);
        if (!format_decimal(line,sizeof(line),(g_hw_debug.tc_valid ? g_hw_debug.tc_display_c*d->tc_gain+d->tc_offset_c : d->temperature_c),1))
            (void)snprintf(line,sizeof(line),"--");
        oled_text_big(0,14,line); oled_text(98,21,"C");
        number(33,d->vdda_fallback?"VF EST: ":"VF V: ",d->vf_voltage,3);
        (void)snprintf(line,sizeof(line),"SD:%s CAL:%s",d->storage_ready?"OK":"OFF",
                       d->calibration_valid?"OK":"NAO"); oled_text(0,43,line);
        oled_text(0,55,"CLIQUE: MENU"); break;
    case UI_SCREEN_MENU:
        title("CARACTERIZACAO");
        choice(11,"ENSAIO",0);choice(20,"CALIBRACAO",1);
        choice(29,"CAL TERMOPAR",2);choice(38,"ENSAIO K MANUAL",3);
        choice(47,"ENSAIO TSP",4);choice(56,"VOLTAR",5);break;
    case UI_SCREEN_SOFIA_SETUP: {
        title("TSP: DEGRAU POTENCIA");
        unsigned first=selected>4?selected-4:0;
        for(unsigned row=first;row<7 && row<first+5;row++) {
            if(row<5) {
                char v[18];float value=row==0?(float)sofia_cfg.heat_s:row==1?sofia_cfg.slope:row==2?sofia_cfg.intercept:row==3?(float)sofia_cfg.delay_us:sofia_cfg.max_case_c;
                (void)format_decimal(v,sizeof(v),value,(row==1 || row==2)?2:0);
                (void)snprintf(line,sizeof(line),"%s %.16s",row==0?"TEMPO s":row==1?"M C/V":row==2?"B C":row==3?"ATRASO us":"MAX CASE C",v);
                choice(13+(int)(row-first)*10,line,row);
            } else choice(13+(int)(row-first)*10,row==5?"REVISAR/INICIAR":"VOLTAR",row);
        }
        break;
    }
    case UI_SCREEN_SOFIA_CONFIRM:
        title("CONFERIR RETA T(V)");oled_text(0,13,"Ih:758mA Im:2mA");
        oled_text(0,24,"MAX31855 NA CASE");
        choice(39,"INICIAR DEGRAU",0);choice(53,"VOLTAR",1);break;
    case UI_SCREEN_K_MANUAL: {
        title(k_notice?"CONFIRME VALOR K":"K ASSINADO C/V");
        char value[20];(void)format_decimal(value,sizeof(value),manual_k,1);
        (void)snprintf(line,sizeof(line),"K: %s",value);choice(14,line,0);
        (void)format_decimal(value,sizeof(value),k_steps[k_step_index],0);
        (void)snprintf(line,sizeof(line),"PASSO: %s",value);choice(26,line,1);
        choice(39,"CONFIG ENSAIO",2);choice(52,"VOLTAR",3);break;
    }
    case UI_SCREEN_TC_CAL:
        title("CAL TERMOPAR");choice(11,"3 TEMPERATURAS",0);choice(20,"4 TEMPERATURAS",1);
        choice(29,"AUTOAQUECER",2);choice(38,"OFFSET/FILTRO",3);
        choice(47,"RESET TOTAL TC",4);choice(56,"VOLTAR",5);break;
    case UI_SCREEN_TC_HEAT_SETUP:
        title("AUTOAQUECER TERMOPAR");
        (void)snprintf(line,sizeof(line),"PONTOS: %u",tc_heat_points);choice(12,line,0);
        { char v[16];(void)format_decimal(v,sizeof(v),tc_heat_first,0);
          (void)snprintf(line,sizeof(line),"INICIO: %s C",v);choice(22,line,1);
          (void)format_decimal(v,sizeof(v),tc_heat_step,0);
          (void)snprintf(line,sizeof(line),"PASSO: %s C",v);choice(32,line,2); }
        choice(43,"INICIAR AQUECIMENTO",3);choice(54,"VOLTAR",4);break;
    case UI_SCREEN_TC_MULTI:
        title(d->tc_heat_error?"FALHA AQUECIMENTO":tc_auto && tc_ref_notice?"CONFIRME REF DIGITAL":d->tc_capture_busy?"COLETANDO 4 SEG":
              d->tc_adjust_status==4?"INSTAVEL: REPITA":d->tc_adjust_status==8?"AGUARDE ESTAVEL":
              d->tc_heat_active?(d->tc_heat_waiting_next?"PONTO OK: PROXIMO?":"DIGITAL NO ALVO?"):
              d->tc_adjust_status==1?"TERMOPAR: SALVO":
              d->tc_adjust_status==2?"AJUSTE INVALIDO":
              d->tc_adjust_status==3?"FALHA CARTAO SD":
              d->tc_adjust_status==4?"INSTAVEL: REPITA":
              d->tc_adjust_status==6?"AJUSTE PRONTO":"REF DO TERM DIGITAL");
        {
            char numeric[18];(void)format_decimal(numeric,sizeof(numeric),tc_reference,1);
            if(tc_auto) {
                (void)format_decimal(numeric,sizeof(numeric),d->tc_heat_target_c,1);
                (void)snprintf(line,sizeof(line),"ALVO DIGITAL: %.10s C",numeric);oled_text(0,13,line);
            } else {(void)snprintf(line,sizeof(line),"REF: %s C",numeric);choice(13,line,0);}
        }
        if(d->tc_candidate_valid) {
            number(23,"GANHO: ",d->tc_gain,4);number(33,"R2: ",d->tc_r2,4);
        } else {
            number(23,"BRUTA: ",g_hw_debug.tc_valid?g_hw_debug.tc_raw_c:NAN,2);
            if(d->tc_heat_active) {
                (void)snprintf(line,sizeof(line),"PONTOS: %u/%u",d->tc_points,d->tc_required);
            } else (void)snprintf(line,sizeof(line),"PT:%u/%u AM:%u/16",d->tc_points,d->tc_required,d->tc_capture_count);
            oled_text(0,33,line);
        }
        choice(43,tc_auto && d->tc_heat_active?(d->tc_heat_waiting_next?"PROXIMO ALVO":"CONFIRMAR ALVO"):"COLETAR PONTO",1);choice(54,"APLICAR/SALVAR",2);break;
    case UI_SCREEN_TC_FINE:
        title(tc_notice?(d->tc_adjust_status==1?"TERMOPAR: SALVO":d->tc_adjust_status==3?"FALHA CARTAO SD":"AJUSTE INVALIDO"):"OFFSET / FILTRO");
        {
            char value[18];(void)format_decimal(value,sizeof(value),tc_manual_offset,1);
            (void)snprintf(line,sizeof(line),"OFFSET: %s C",value);choice(13,line,0);
            (void)format_decimal(value,sizeof(value),tc_alpha,2);
            (void)snprintf(line,sizeof(line),"FILTRO: %s",value);choice(23,line,1);
        }
        number(33,"GANHO: ",d->tc_gain,4);choice(43,"APLICAR/SALVAR",2);choice(54,"ZERAR AJUSTES",3);break;
    case UI_SCREEN_TC_RESET:
        title(tc_notice?(d->tc_adjust_status==7?"TERMOPAR RESETADO":"FALHA CARTAO SD"):"RESET TOTAL TERMOPAR");
        oled_text(0,13,"APAGA AJUSTE 3/4 PT");
        oled_text(0,24,"GANHO/OFFSET/FILTRO");
        choice(39,"CONFIRMAR RESET",0);choice(53,"CANCELAR",1);break;
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
        title(screen == UI_SCREEN_CONFIRM_CAL ? "CALIBRACAO" :
              (manual_k_mode?"ENSAIO K MANUAL":"CONFIRMAR ENSAIO"));
        oled_text(0,14,manual_k_mode?"DUT FRIO/EQUILIBRIO":"CONFERIR PARAMETROS");
        if(manual_k_mode)number(25,"K C/V: ",manual_k,1);
        else oled_text(0,25,d->power_control_enabled?"POTENCIA HABILITADA":"RELE BLOQUEADO");
        choice(39,"INICIAR",0); choice(52,"CANCELAR",1); break;
    case UI_SCREEN_STATUS:
        title("DIAGNOSTICO"); number(13,"T C: ",(g_hw_debug.tc_valid ? g_hw_debug.tc_display_c*d->tc_gain+d->tc_offset_c : d->temperature_c),1);
        number(23,"VF V: ",d->vf_voltage,3);
        (void)snprintf(line,sizeof(line),"SD:%s CAL:%s",d->storage_ready?"OK":"OFF",
                       d->calibration_valid?"OK":"NAO");
        oled_text(0,35,line);
        oled_text(0,46,d->power_control_enabled?"POTENCIA HABILITADA":"POTENCIA BLOQUEADA");
        oled_text(0,56,"CLIQUE: VOLTAR"); break;
    case UI_SCREEN_RUNNING:
        (void)snprintf(line,sizeof(line),"%s %02lu:%02lu:%02lu",d->sofia_active?(d->sofia_phase==2?"TSP RESFR":"TSP AQUEC"):d->calibrating?"CAL":d->manual_k_active?"HEAT K":"HEAT",
            (unsigned long)(d->elapsed_seconds/3600U),
            (unsigned long)((d->elapsed_seconds/60U)%60U),
            (unsigned long)(d->elapsed_seconds%60U)); title(line);
        number(13,d->sofia_active?"CASE C: ":"T C: ",(g_hw_debug.tc_valid ? g_hw_debug.tc_display_c*d->tc_gain+d->tc_offset_c : d->temperature_c),1);
        number(23,d->sofia_active?"LIM CASE: ":"ALVO: ",d->target_temperature_c,1); number(33,"VF: ",d->vf_voltage,3);
        unsigned progress = d->progress_percent > 100 ? 100 : d->progress_percent;
        oled_rect(0,44,128,6,false); oled_rect(1,45,(int)(126U*progress/100U),4,true);
        oled_text(0,55,d->sofia_active && d->sofia_phase==1?"CLIQUE: MEDIR AGORA":"SEGURAR: ABORTAR"); break;
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
            d->error_code == 4 ? "FALHA MEDICAO VF" :
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
