#include "app.h"
#include "hw_debug.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <string.h>

#include "board.h"
#include "device.h"
#include "encoder.h"
#include "job.h"
#include "sofia.h"
#include "oled.h"
#include "power.h"
#include "rtd.h"
#include "sdlog.h"
#include "sd_io.h"
#include "ui.h"
#include "vf_adc.h"
#include "calibration.h"
#include "tc_calibration.h"
#include "tc_heater.h"
#include "runtime_config.h"

/*
 * Coordenador principal da aplicação.
 * Mantém o estado compartilhado e agenda tarefas periódicas cooperativas.
 * SPI/I2C possuem timeout curto; FatFs pode bloquear e requer revisão no alvo.
 * Drivers e processos são chamados por interfaces; a UI não controla hardware.
 */
#define RTD_PERIOD_MS       250U
#define UI_RENDER_PERIOD_MS 100U

static device_t s_device;
static bool s_job_active;
static uint32_t s_last_rtd_ms;
static uint32_t s_last_render_ms;
static bool s_temperature_valid;
const device_t *app_device(void) { return &s_device; }

static void app_update_measurements(uint32_t now_ms)
{
    float value;

    if ((uint32_t)(now_ms - s_last_rtd_ms) < RTD_PERIOD_MS) {
        return;
    }
    s_last_rtd_ms = now_ms;

    if (rtd_read_temperature(&value)) {
        s_device.temperature_c = value;
        g_hw_debug.tc_corrected_c=value;
        g_hw_debug.tc_display_corrected_c=tc_calibration_apply(g_hw_debug.tc_display_c);
        s_temperature_valid = true;
    } else {
        s_temperature_valid = false;
        s_device.temperature_c = NAN;
        g_hw_debug.tc_corrected_c=NAN;
        g_hw_debug.tc_display_corrected_c=NAN;
    }
    s_device.sensor_sequence++;

    if (vf_adc_read_voltage(&value)) {
        s_device.vf_voltage = value;
    } else s_device.vf_voltage = NAN;
    s_device.vdda_v = vf_adc_vdda();
    s_device.vdda_fallback = vf_adc_vdda_fallback();
}

static struct {
 float raw[4],reference[4],sum,low,high,reference_pending;
 unsigned count,required,samples;
 uint32_t sequence,started;
 bool collecting;
 tc_cal_params_t candidate;
} tc_session;
static void tc_sync(void) {
 const tc_cal_params_t *p=tc_calibration_get();
 s_device.tc_offset_c=p->offset;s_device.tc_gain=p->gain;s_device.tc_r2=p->r2;
 g_hw_debug.tc_offset_c=p->offset;g_hw_debug.tc_gain=p->gain;
 g_hw_tuning.display_filter_alpha=p->alpha;
 if (g_hw_debug.tc_valid) {
  s_device.temperature_c=tc_calibration_apply(g_hw_debug.tc_raw_c);
  g_hw_debug.tc_corrected_c=s_device.temperature_c;
  g_hw_debug.tc_display_corrected_c=tc_calibration_apply(g_hw_debug.tc_display_c);
 }
}
static void tc_apply_saved(const tc_cal_params_t *p) {
 const tc_cal_params_t previous=*tc_calibration_get();
 (void)tc_calibration_set(p);tc_sync();
 if(fabsf(previous.gain-p->gain)>0.00001f || fabsf(previous.offset-p->offset)>0.0002f) {
  cal_params_t invalid=*calibration_get();invalid.valid=false;calibration_set(&invalid);
  s_device.calibration=invalid;s_device.calibration_valid=0;
 }
}
static bool tc_save(const tc_cal_params_t *p) {
 if (!tc_calibration_valid(p)) {s_device.tc_adjust_status=2;return false;}
 if (!sdlog_ready() || !sdlog_save_tc_calibration(p)) {s_device.tc_adjust_status=3;return false;}
 tc_apply_saved(p);s_device.tc_adjust_status=1;return true;
}
static void tc_heat_sync(void) {
 s_device.tc_heat_holding=0;s_device.tc_heat_waiting_next=g_tc_heat_debug.waiting_next;
 s_device.tc_heat_active=g_tc_heat_debug.active;s_device.tc_heat_stable=0;
 s_device.tc_heat_error=(uint8_t)g_tc_heat_debug.error;s_device.tc_heat_target_c=g_tc_heat_debug.target_c;
}
static void tc_accept_point(float raw,float reference) {
 unsigned i=tc_session.count;
 tc_session.raw[i]=raw;
 tc_session.reference[i]=reference;
 tc_session.count++;s_device.tc_points=(uint8_t)tc_session.count;
 tc_heater_advance();tc_heat_sync();
 s_device.tc_adjust_status=5;
 if(tc_session.count==tc_session.required) {
  bool valid=tc_calibration_fit(tc_session.raw,tc_session.reference,tc_session.count,&tc_session.candidate);
  s_device.tc_candidate_valid=valid;
  if(valid){s_device.tc_gain=tc_session.candidate.gain;s_device.tc_r2=tc_session.candidate.r2;s_device.tc_adjust_status=6;}
  else s_device.tc_adjust_status=2;
 }
}
static void tc_collect(uint32_t now) {
 if (!tc_session.collecting) return;
 if ((uint32_t)(now-tc_session.started)>10000U || !g_hw_debug.tc_valid) {
  tc_session.collecting=false;s_device.tc_capture_busy=0;s_device.tc_adjust_status=2;return;
 }
 if (g_hw_debug.tc_samples==tc_session.sequence) return;
 tc_session.sequence=g_hw_debug.tc_samples;
 float raw=g_hw_debug.tc_raw_c;
 if (!isfinite(raw)) return;
 tc_session.sum+=raw;
 if(raw<tc_session.low)tc_session.low=raw;
 if(raw>tc_session.high)tc_session.high=raw;
 s_device.tc_capture_count=(uint8_t)++tc_session.samples;
 if(tc_session.samples<TC_CAPTURE_SAMPLES)return;
 tc_session.collecting=false;s_device.tc_capture_busy=0;
 if(tc_session.high-tc_session.low>TC_CAPTURE_MAX_SPAN_C) {s_device.tc_adjust_status=4;return;}
 tc_accept_point(tc_session.sum/TC_CAPTURE_SAMPLES,tc_session.reference_pending);
}

static bool app_start_command(const ui_cmd_t *cmd)
{
    if (cmd == NULL) {
        return false;
    }

    if ((cmd->type == UI_CMD_START_CALIBRATION ||
         cmd->type == UI_CMD_START_TEST || cmd->type == UI_CMD_START_TEST_K || cmd->type==UI_CMD_START_SOFIA) &&
        (!s_temperature_valid || s_job_active || tc_session.collecting || g_tc_heat_debug.active ||
          s_device.temperature_c>=OVERTEMP_C || (g_hw_debug.tc_valid && g_hw_debug.tc_raw_c>=OVERTEMP_C))) {
        if (!s_job_active) {
            tc_heater_stop();tc_heat_sync();tc_session.collecting=false;s_device.tc_capture_busy=0;
            s_device.mode = DEVICE_ERROR; s_device.error_code = JOB_ERROR_SENSOR;
        }
        return false;
    }

    switch (cmd->type) {
    case UI_CMD_TC_BEGIN:
    case UI_CMD_TC_HEAT_BEGIN:
        if(s_job_active || (cmd->tc_points!=3 && cmd->tc_points!=4))return false;
        tc_heater_stop();g_tc_heat_debug.error=0;tc_heat_sync();tc_session.collecting=false;tc_session.count=0;tc_session.required=cmd->tc_points;
        s_device.tc_points=0;s_device.tc_required=(uint8_t)cmd->tc_points;
        s_device.tc_capture_busy=0;s_device.tc_capture_count=0;s_device.tc_candidate_valid=0;s_device.tc_adjust_status=0;
        if(cmd->type==UI_CMD_TC_HEAT_BEGIN) {
            if(!tc_heater_start(cmd->tc_points,cmd->start_temperature_c,cmd->step_temperature_c,
                 s_device.temperature_c,g_hw_debug.tc_raw_c,s_temperature_valid && g_hw_debug.tc_valid,g_hw_debug.tc_last_ok_ms)) {
                s_device.tc_adjust_status=9;tc_heat_sync();return false;
            }
            tc_heat_sync();
        }
        return true;
    case UI_CMD_TC_HEAT_NEXT:
        if(s_job_active || tc_session.collecting || !s_temperature_valid || !g_hw_debug.tc_valid ||
           (uint32_t)(board_millis()-g_hw_debug.tc_last_ok_ms)>1000U ||
           s_device.temperature_c>=OVERTEMP_C || g_hw_debug.tc_raw_c>=OVERTEMP_C)return false;
        {bool ok=tc_heater_next();tc_heat_sync();s_device.tc_adjust_status=ok?0:9;return ok;}
    case UI_CMD_TC_CAPTURE:
        if(s_job_active || tc_session.collecting || tc_session.count>=tc_session.required || tc_session.required<3)return false;
        if(g_tc_heat_debug.error) {s_device.tc_adjust_status=9;return false;}
        if(g_tc_heat_debug.active) {
            if(g_tc_heat_debug.waiting_next)return false;
            /* Stop immediately, then associate the latest fresh raw reading
             * with the preset target confirmed on the independent instrument. */
            power_safe();g_tc_heat_debug.heating=false;
            if(!g_hw_debug.tc_valid || !isfinite(g_hw_debug.tc_raw_c) ||
               (uint32_t)(board_millis()-g_hw_debug.tc_last_ok_ms)>TC_HEAT_CONFIRM_MAX_AGE_MS) {
                tc_heater_stop();tc_heat_sync();s_device.tc_adjust_status=2;return false;
            }
            s_device.tc_capture_count=1;
            tc_accept_point(g_hw_debug.tc_raw_c,g_tc_heat_debug.target_c);
            return true;
        }
        if(!g_tc_heat_debug.active)power_safe();
        if(!g_hw_debug.tc_valid || (uint32_t)(board_millis()-g_hw_debug.tc_last_ok_ms)>1000U ||
           !isfinite(cmd->reference_temperature_c) || cmd->reference_temperature_c<TEMP_MIN_C || cmd->reference_temperature_c>TEMP_MAX_C) {
            s_device.tc_adjust_status=2;return false;
        }
        tc_session.reference_pending=cmd->reference_temperature_c;
        tc_session.sum=0;tc_session.low=INFINITY;tc_session.high=-INFINITY;tc_session.samples=0;
        tc_session.sequence=g_hw_debug.tc_samples;tc_session.started=board_millis();tc_session.collecting=true;
        s_device.tc_capture_busy=1;s_device.tc_capture_count=0;s_device.tc_adjust_status=0;return true;
    case UI_CMD_TC_CANCEL:
        tc_heater_stop();tc_heat_sync();tc_session.collecting=false;s_device.tc_capture_busy=0;tc_sync();return true;
    case UI_CMD_TC_APPLY_FIT:
        if(s_job_active || !s_device.tc_candidate_valid || tc_session.collecting)return false;
        tc_heater_stop();tc_heat_sync();return tc_save(&tc_session.candidate);
    case UI_CMD_TC_SAVE: {
        if(s_job_active || tc_session.collecting)return false;
        tc_heater_stop();tc_heat_sync();tc_cal_params_t p=*tc_calibration_get();
        p.offset=cmd->tc_offset;p.alpha=cmd->tc_alpha;p.points=0;p.r2=1;
        return tc_save(&p);
    }
    case UI_CMD_TC_RESET: {
        if(s_job_active)return false;
        tc_heater_stop();tc_heat_sync();
        memset(&tc_session,0,sizeof(tc_session));
        s_device.tc_points=0;s_device.tc_required=0;s_device.tc_capture_count=0;
        s_device.tc_capture_busy=0;s_device.tc_candidate_valid=0;
        if(!sdlog_ready() || !sdlog_reset_tc_calibration()) {s_device.tc_adjust_status=3;tc_sync();return false;}
        const tc_cal_params_t p={1,0,THERMAL_DISPLAY_FILTER_ALPHA,1,0};
        tc_apply_saved(&p);
        rtd_init();s_temperature_valid=false;s_device.temperature_c=NAN;
        g_hw_debug.tc_corrected_c=NAN;g_hw_debug.tc_display_corrected_c=NAN;
        s_device.tc_adjust_status=7;return true;
    }
    case UI_CMD_START_SOFIA:
        if(sofia_start(&cmd->sofia,&s_device)) {
            s_job_active=true;s_device.sofia_active=1;s_device.sofia_phase=1;
            s_device.calibrating=0;s_device.manual_k_active=0;s_device.mode=DEVICE_RUNNING;
            s_device.progress_percent=0;s_device.error_code=0;return true;
        }
        s_device.mode=DEVICE_ERROR;s_device.error_code=(int32_t)g_sofia_debug.error;power_safe();return false;
    case UI_CMD_SOFIA_MEASURE:
        if(s_job_active && s_device.sofia_active){sofia_measure_now();return true;}return false;
    case UI_CMD_START_CALIBRATION:
        if (job_start_calibration_params(cmd->start_temperature_c,cmd->end_temperature_c,cmd->step_temperature_c)) {
            s_job_active = true;
            s_device.mode = DEVICE_RUNNING;
            s_device.progress_percent = 0U;
            s_device.error_code = 0;
            s_device.sofia_active=0;s_device.calibrating = 1; s_device.manual_k_active=0;
            return true;
        }
        break;

    case UI_CMD_START_TEST:
    case UI_CMD_START_TEST_K:
        if (cmd->type==UI_CMD_START_TEST_K?
            job_start_test_manual_k(cmd->start_temperature_c,cmd->end_temperature_c,cmd->step_temperature_c,
                                   cmd->manual_k_c_per_v,s_device.temperature_c):
            job_start_test(cmd->start_temperature_c,cmd->end_temperature_c,cmd->step_temperature_c)) {
            s_job_active = true;
            s_device.mode = DEVICE_RUNNING;
            s_device.target_temperature_c = cmd->start_temperature_c;
            s_device.progress_percent = 0U;
            s_device.error_code = 0;
            s_device.sofia_active=0;s_device.calibrating = 0; s_device.manual_k_active=cmd->type==UI_CMD_START_TEST_K;
            return true;
        }
        break;

    case UI_CMD_ABORT:
        tc_heater_stop();tc_heat_sync();
        if (s_job_active) {
            if(s_device.sofia_active)sofia_abort();else job_abort();
            s_job_active = false;
            power_safe();
            s_device.mode = DEVICE_ABORTED;
            s_device.progress_percent = 0U;
        } else {
            power_safe();
        }
        return true;

    case UI_CMD_RETURN_HOME:
        tc_heater_stop();tc_heat_sync();tc_session.collecting=false;s_device.tc_capture_busy=0;
        if (!s_job_active) { s_device.mode = DEVICE_STANDBY; s_device.error_code = 0; }
        return true;
    case UI_CMD_NONE:
    default:
        break;
    }

    if (cmd->type == UI_CMD_START_CALIBRATION || cmd->type == UI_CMD_START_TEST ||
        cmd->type == UI_CMD_START_TEST_K) {
        s_device.mode = DEVICE_ERROR; s_device.error_code = job_error();
        power_safe();
    }
    return false;
}

void app_init(void)
{
    if (!board_is_ready()) { board_force_safe(); return; }
    s_device=(device_t){0};
    tc_session.collecting=false;tc_session.count=tc_session.required=tc_session.samples=0;
    s_device.mode = DEVICE_STANDBY;
    s_device.power_control_enabled = board_power_control_enabled();
    s_device.temperature_c = NAN;
    s_device.target_temperature_c = 0.0f;
    s_device.vf_voltage = NAN;
    s_device.calibration_valid = 0U;
    s_device.progress_percent = 0U;
    s_device.elapsed_seconds = 0U;
    s_device.error_code = 0;
    s_job_active = false;
    s_temperature_valid = false;

    /* Inicializa módulos; board_init() já foi chamado por main.c. */
    power_init();
    tc_heater_stop();g_tc_heat_debug.error=0;tc_heat_sync();
    power_safe(); /* redundância antes dos demais drivers */
    tc_calibration_init();
    s_device.tc_adjust_status=0;
    s_device.tc_offset_c=0;
    rtd_init();
    vf_adc_init();
    oled_init();
    encoder_init();
    sdlog_init();
    tc_cal_params_t tc_saved;
    if (sdlog_load_tc_calibration(&tc_saved)) (void)tc_calibration_set(&tc_saved);
    tc_sync();
    calibration_init();
    s_device.calibration = *calibration_get();
    s_device.calibration_valid = s_device.calibration.valid;
    s_device.storage_ready = sdlog_ready();
    job_init();sofia_init();
    ui_init();

    /* Estado seguro obrigatório antes de entrar no superloop. */
    power_safe();

    s_last_rtd_ms = board_millis();
    s_last_render_ms = s_last_rtd_ms;
}

void app_run(void)
{
    if (!board_is_ready()) { tc_heater_stop();tc_heat_sync(); return; }
    uint32_t now_ms = board_millis();
    input_event_t event;
    ui_cmd_t cmd;

    if (!s_job_active && !g_tc_heat_debug.active) oled_recover_service();
    vf_adc_service();
    app_update_measurements(now_ms);
    tc_heater_service(s_device.temperature_c,g_hw_debug.tc_raw_c,s_temperature_valid && g_hw_debug.tc_valid,
                      s_device.sensor_sequence,g_hw_debug.tc_last_ok_ms);
    tc_heat_sync();
    if(g_tc_heat_debug.error) {
        tc_session.collecting=false;s_device.tc_capture_busy=0;s_device.tc_adjust_status=9;
    }
    tc_collect(now_ms);
    s_device.storage_ready = sdlog_ready();

    event = encoder_get_event();
    if (!s_job_active && !g_tc_heat_debug.active && event == INPUT_EVENT_LONG && ui_current_screen()!=UI_SCREEN_TC_CAL && ui_current_screen()!=UI_SCREEN_TC_MULTI && ui_current_screen()!=UI_SCREEN_TC_FINE && ui_current_screen()!=UI_SCREEN_TC_RESET && ui_current_screen()!=UI_SCREEN_TC_HEAT_SETUP) {
        power_safe(); /* Manual storage retry only while idle. */
        (void)board_sd_prepare();
        sdlog_init();
        tc_cal_params_t tc_saved;
        if (sdlog_load_tc_calibration(&tc_saved)) (void)tc_calibration_set(&tc_saved);
        tc_sync();
        calibration_init();
        s_device.calibration = *calibration_get();
        s_device.calibration_valid = s_device.calibration.valid;
        s_device.storage_ready = sdlog_ready();
    }
    cmd = ui_step(&s_device, event);
    (void)app_start_command(&cmd);

    if (s_job_active) {
        job_status_t status = s_device.sofia_active?sofia_step(&s_device):job_step(&s_device);
        if(s_device.sofia_active){s_device.sofia_phase=(uint8_t)g_sofia_debug.phase;s_device.error_code=(int32_t)g_sofia_debug.error;}

        if (status == JOB_STATUS_COMPLETED) {
            s_job_active = false;
            power_safe();
            s_device.mode = DEVICE_RESULT;
            s_device.progress_percent = 100U;
        } else if (status == JOB_STATUS_FAILED) {
            s_job_active = false;
            power_safe();
            s_device.mode = DEVICE_ERROR;
        } else if (status == JOB_STATUS_ABORTED) {
            s_job_active = false; power_safe(); s_device.mode = DEVICE_ABORTED;
        }
    }

    now_ms = board_millis();
    if ((uint32_t)(now_ms - s_last_render_ms) >= UI_RENDER_PERIOD_MS) {
        s_last_render_ms = now_ms;
        ui_render(&s_device);
    }
    oled_service();
    board_debug_service();
}
