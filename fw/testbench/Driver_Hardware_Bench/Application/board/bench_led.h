#ifndef BENCH_LED_H
#define BENCH_LED_H
#include <stdbool.h>
#include <stdint.h>
bool board_bench_led_init(void);
void board_bench_led_write(bool on);
uint32_t board_bench_core_clock(void);
#endif
