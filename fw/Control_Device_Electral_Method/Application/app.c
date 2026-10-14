#include "app.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <math.h>

#include "board.h"
#include "device.h"
#include "encoder.h"
#include "job.h"
#include "oled.h"
#include "power.h"
#include "rtd.h"
#include "sdlog.h"
#include "ui.h"
#include "vf_adc.h"
#include "calibration.h"

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
        s_temperature_valid = true;
    } else {
        s_temperature_valid = false;
        s_device.temperature_c = NAN;
    }
    s_device.sensor_sequence++;

    if (vf_adc_read_voltage(&value)) {
        s_device.vf_voltage = value;
    } else s_device.vf_voltage = NAN;
    s_device.vdda_v = vf_adc_vdda();
    s_device.vdda_fallback = vf_adc_vdda_fallback();
}

static bool app_start_command(const ui_cmd_t *cmd)
{
    if (cmd == NULL) {
        return false;
    }

    if ((cmd->type == UI_CMD_START_CALIBRATION ||
         cmd->type == UI_CMD_START_TEST) &&
        (!s_temperature_valid || s_job_active)) {
        if (!s_job_active) {
            s_device.mode = DEVICE_ERROR; s_device.error_code = JOB_ERROR_SENSOR;
        }
        return false;
    }

    switch (cmd->type) {
    case UI_CMD_START_CALIBRATION:
        if (job_start_calibration_params(cmd->start_temperature_c,
                                         cmd->end_temperature_c,
                                         cmd->step_temperature_c)) {
            s_job_active = true;
            s_device.mode = DEVICE_RUNNING;
            s_device.progress_percent = 0U;
            s_device.error_code = 0;
            s_device.calibrating = 1;
            return true;
        }
        break;

    case UI_CMD_START_TEST:
        if (job_start_test(cmd->start_temperature_c,
                           cmd->end_temperature_c,
                           cmd->step_temperature_c)) {
            s_job_active = true;
            s_device.mode = DEVICE_RUNNING;
            s_device.target_temperature_c = cmd->start_temperature_c;
            s_device.progress_percent = 0U;
            s_device.error_code = 0;
            s_device.calibrating = 0;
            return true;
        }
        break;

    case UI_CMD_ABORT:
        if (s_job_active) {
            job_abort();
            s_job_active = false;
            power_safe();
            s_device.mode = DEVICE_ABORTED;
            s_device.progress_percent = 0U;
        } else {
            power_safe();
        }
        return true;

    case UI_CMD_RETURN_HOME:
        if (!s_job_active) { s_device.mode = DEVICE_STANDBY; s_device.error_code = 0; }
        return true;
    case UI_CMD_NONE:
    default:
        break;
    }

    if (cmd->type == UI_CMD_START_CALIBRATION || cmd->type == UI_CMD_START_TEST) {
        s_device.mode = DEVICE_ERROR; s_device.error_code = job_error();
        power_safe();
    }
    return false;
}

void app_init(void)
{
    if (!board_is_ready()) { board_force_safe(); return; }
    s_device.mode = DEVICE_STANDBY;
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
    power_safe(); /* redundância antes dos demais drivers */
    rtd_init();
    vf_adc_init();
    oled_init();
    encoder_init();
    sdlog_init();
    calibration_init();
    s_device.calibration = *calibration_get();
    s_device.calibration_valid = s_device.calibration.valid;
    s_device.storage_ready = sdlog_ready();
    job_init();
    ui_init();

    /* Estado seguro obrigatório antes de entrar no superloop. */
    power_safe();

    s_last_rtd_ms = board_millis();
    s_last_render_ms = s_last_rtd_ms;
}

void app_run(void)
{
    if (!board_is_ready()) { power_safe(); return; }
    uint32_t now_ms = board_millis();
    input_event_t event;
    ui_cmd_t cmd;

    vf_adc_service();
    app_update_measurements(now_ms);
    s_device.storage_ready = sdlog_ready();

    event = encoder_get_event();
    cmd = ui_step(&s_device, event);
    (void)app_start_command(&cmd);

    if (s_job_active) {
        job_status_t status = job_step(&s_device);

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
}
