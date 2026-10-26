#ifndef BENCH_PINOUT_H
#define BENCH_PINOUT_H
#include "stm32h7xx_hal.h"
/* NUCLEO-H753ZI / MB1364, UM2407 tables 13 and LEDs section.
 * LD1 uses PB0 when SB39 ON/SB47 OFF. Confirm the actual board bridges. */
#define BENCH_LED_GREEN_PORT GPIOB
#define BENCH_LED_GREEN_PIN GPIO_PIN_0
#define BENCH_LED_YELLOW_PORT GPIOE
#define BENCH_LED_YELLOW_PIN GPIO_PIN_1
#define BENCH_LED_RED_PORT GPIOB
#define BENCH_LED_RED_PIN GPIO_PIN_14
#define BENCH_ETH_PA_PINS (GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_7)
#define BENCH_ETH_PC_PINS (GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5)
#define BENCH_ETH_PG_PINS (GPIO_PIN_11|GPIO_PIN_13)
#define BENCH_ETH_PB_PINS GPIO_PIN_13
/* No relay, SELECT, DAC, heater or DUT source pins are configured here. */
#endif
