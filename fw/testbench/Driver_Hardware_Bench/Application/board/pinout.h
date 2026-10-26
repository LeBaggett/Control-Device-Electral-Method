#ifndef THERMAL_PINOUT_H
#define THERMAL_PINOUT_H
/* Mapa centralizado. Alterar SOMENTE aqui quando o hardware mudar.
 * Os GPIOx e GPIO_PIN_x vêm da HAL STM32F4. Não incluir este arquivo em core/ui.
 * Pinos do planejamento original mantidos; MAX31855 substitui MAX31865.
 */
#include "stm32f4xx_hal.h"

#include "peripheral_io.h"
/* Display selection: change this one definition, then Clean + Build.
 * SSD1306: 0.96-inch 128x64 tested by owner; SH1106: 1.3-inch 128x64.
 * Controller identity/resolution matter, not physical size alone. */
#ifndef OLED_SELECTED_DRIVER
#define OLED_SELECTED_DRIVER OLED_DRIVER_SSD1306
#endif
#define OLED_SH1106_COLUMN_OFFSET 2U
#if OLED_SELECTED_DRIVER != OLED_DRIVER_SSD1306 && OLED_SELECTED_DRIVER != OLED_DRIVER_SH1106
#error "Select OLED_DRIVER_SSD1306 or OLED_DRIVER_SH1106"
#endif
#if OLED_SH1106_COLUMN_OFFSET > 4U
#error "SH1106 offset must fit its 132-column RAM"
#endif


/* Built-in LED confirmed by owner: PA1, active LOW. */
#ifndef BENCH_LED_CONFIGURED
#define BENCH_LED_CONFIGURED 1
#endif
#define BENCH_LED_PORT GPIOA
#define BENCH_LED_PIN GPIO_PIN_1
#define BENCH_LED_ACTIVE_LEVEL GPIO_PIN_RESET
#define BENCH_LED_CLOCK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#if BENCH_LED_CONFIGURED
#if !defined(BENCH_LED_PORT) || !defined(BENCH_LED_PIN) || \
    !defined(BENCH_LED_ACTIVE_LEVEL) || !defined(BENCH_LED_CLOCK_ENABLE)
#error "Confirm board LED GPIO, active level and GPIO clock before enabling LED"
#endif
#endif


/* 0: relé OFF e SELECT em medição; recusa energização/aquecimento.
 * 1: permite os comandos após board_init e na ordem segura.
 * Alterar somente após testes e recompilar; não energiza no boot. */
#ifndef BOARD_POWER_CONTROL_ENABLED
#define BOARD_POWER_CONTROL_ENABLED 1
#endif
#if BOARD_POWER_CONTROL_ENABLED != 0 && BOARD_POWER_CONTROL_ENABLED != 1
#error "Power control flag must be 0 or 1"
#endif

/* Segurança: PD14=RELAY_EN ativo alto; PD12=CUR_SEL alto=medição. */
#define PIN_RELAY_PORT GPIOD
#define PIN_RELAY       GPIO_PIN_14
#define PIN_SELECT_PORT GPIOD
#define PIN_SELECT       GPIO_PIN_12
#define PIN_RELAY_CLOCK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define PIN_SELECT_CLOCK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define PIN_RELAY_OFF    GPIO_PIN_RESET
#define PIN_RELAY_ON     GPIO_PIN_SET
#define PIN_SELECT_PULL GPIO_PULLDOWN /* Internal bias only; initial ODR remains measurement HIGH. */
#define PIN_SELECT_MEAS GPIO_PIN_SET
#define PIN_SELECT_HEAT GPIO_PIN_RESET

/* MAX31855: SPI1 mode 0; PA7 reservado ao MOSI do planejamento, não usado pelo CI. */
#define PIN_TC_CS_PORT GPIOA
#define PIN_TC_CS      GPIO_PIN_4
#define PIN_TC_CS_CLOCK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define PIN_TC_SCK_PORT GPIOA
#define PIN_TC_SCK      GPIO_PIN_5
#define PIN_TC_MISO_PORT GPIOA
#define PIN_TC_MISO      GPIO_PIN_6
#define PIN_TC_MOSI_RESERVED_PORT GPIOA
#define PIN_TC_MOSI_RESERVED GPIO_PIN_7

/* ADC Vf: ADC1_IN10 */
#define PIN_VF_PORT GPIOC
#define PIN_VF      GPIO_PIN_0
#define PIN_VF_ADC_CHANNEL ADC_CHANNEL_10

/* OLED: I2C1, endereço 7 bits 0x3C */
#define PIN_OLED_SCL_PORT GPIOB
#define PIN_OLED_SCL GPIO_PIN_8
#define PIN_OLED_SDA_PORT GPIOB
#define PIN_OLED_SDA GPIO_PIN_9
#define OLED_I2C_ADDRESS_7BIT 0x3CU

#define BOARD_ENCODER_PULLUPS_ENABLED 1

/* Encoder: revised PCB layout authorized by owner: PA15/PB3, TIM2. */
#define PIN_ENC_A_PORT GPIOA
#define PIN_ENC_A GPIO_PIN_15
#define PIN_ENC_B_PORT GPIOB
#define PIN_ENC_B GPIO_PIN_3
#define PIN_ENC_SW_PORT GPIOE
#define PIN_ENC_SW GPIO_PIN_2

/* SDIO 4 bits */
#define PIN_SD_D0_PORT GPIOC
#define PIN_SD_D0 GPIO_PIN_8
#define PIN_SD_D1_PORT GPIOC
#define PIN_SD_D1 GPIO_PIN_9
#define PIN_SD_D2_PORT GPIOC
#define PIN_SD_D2 GPIO_PIN_10
#define PIN_SD_D3_PORT GPIOC
#define PIN_SD_D3 GPIO_PIN_11
#define PIN_SD_CK_PORT GPIOC
#define PIN_SD_CK GPIO_PIN_12
#define PIN_SD_CMD_PORT GPIOD
#define PIN_SD_CMD GPIO_PIN_2

#endif
