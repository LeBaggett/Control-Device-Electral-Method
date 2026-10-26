#ifndef OLED_TEST_PINOUT_H
#define OLED_TEST_PINOUT_H
#include "stm32f4xx_hal.h"
#define OLED_SCL_PORT GPIOB
#define OLED_SCL_PIN GPIO_PIN_8
#define OLED_SDA_PORT GPIOB
#define OLED_SDA_PIN GPIO_PIN_9
#define OLED_GPIO_CLOCK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define OLED_GPIO_AF GPIO_AF4_I2C1
#define OLED_BUS_HZ 100000U
/* Weak diagnostic pull-ups; external pull-ups to 3.3V are still required. */
#define OLED_GPIO_PULL GPIO_PULLUP
#define OLED_CONTROLLER_SSD1306 1
#define OLED_CONTROLLER_SH1106 2
#define OLED_CONTROLLER OLED_CONTROLLER_SSD1306
#if OLED_CONTROLLER != 1 && OLED_CONTROLLER != 2
#error "Select SSD1306 or SH1106"
#endif
#define LED_PORT GPIOA
#define LED_PIN GPIO_PIN_1
/* Power IOs are held in the existing safe state; no control commands. */
#define SELECT_PORT GPIOD
#define SELECT_PIN GPIO_PIN_12
#define RELAY_PORT GPIOD
#define RELAY_PIN GPIO_PIN_14
#endif
