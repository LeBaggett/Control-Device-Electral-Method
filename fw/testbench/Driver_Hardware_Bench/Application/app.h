#ifndef BENCH_APP_H
#define BENCH_APP_H
#include <stdint.h>
typedef enum {
 BENCH_CMD_NONE=0, BENCH_CMD_RELAY_ON=1, BENCH_CMD_RELAY_OFF=2,
 BENCH_CMD_SELECT_MEAS=3, BENCH_CMD_SELECT_HEAT=4, BENCH_CMD_SAFE=5,
 BENCH_CMD_SD_PROBE=6, BENCH_CMD_LED_ON=7, BENCH_CMD_LED_OFF=8
} bench_command_t;
typedef enum { BENCH_CONTROL_OK=0, BENCH_CONTROL_LOCKED=1, BENCH_CONTROL_SD=5,
 BENCH_CONTROL_UNKNOWN=6, BENCH_CONTROL_CANCELLED=7 } bench_control_result_t;
typedef struct {
 bench_command_t command;
 uint32_t request_id, ack_id;
 bench_control_result_t result;
} bench_control_t;
extern volatile bench_control_t g_bench_control;
typedef enum { BENCH_WAIT, BENCH_OK, BENCH_FAIL, BENCH_SKIP } bench_result_t;
typedef struct {
    uint8_t relay_on, select_heating, led_on, action, page;
    uint16_t adc_raw;
    bench_result_t board, dwt, oled, thermocouple, adc, sd_mount, sd_rw, encoder;
    float temperature_c, vf_v, vdda_v;
    uint32_t cw, ccw, clicks, long_presses, loop_count;
    uint32_t core_clock_hz;
    uint8_t led_code, led_configured, vdda_estimated;
} bench_status_t;
/* Observe in debugger. OK means the stated software check passed, not metrology. */
extern volatile bench_status_t g_bench_status;
void app_init(void);
void app_run(void);
#endif
