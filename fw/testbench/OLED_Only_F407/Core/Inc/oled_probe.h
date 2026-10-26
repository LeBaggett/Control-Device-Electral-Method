#ifndef OLED_PROBE_H
#define OLED_PROBE_H
#include "main.h"
typedef struct {
 uint32_t magic, version, heartbeat, phase, failure, attempts, retry;
 uint32_t hal_status, hal_error, hal_state, sr1, sr2;
 uint32_t scl_before, sda_before, scl, sda, pulses;
 uint32_t address_7bit, ack_3c, ack_3d, transactions, ready;
 uint32_t gpio_moder, gpio_afrh, core_clock, pclk1;
} oled_test_debug_t;
extern volatile oled_test_debug_t g_oled_test;
extern I2C_HandleTypeDef hi2c1;
void oled_test_run(void);
#endif
