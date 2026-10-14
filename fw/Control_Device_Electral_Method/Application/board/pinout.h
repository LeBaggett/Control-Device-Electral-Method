#ifndef THERMAL_PINOUT_H
#define THERMAL_PINOUT_H
/* Mapa centralizado. Alterar SOMENTE aqui quando o hardware mudar.
 * Os GPIOx e GPIO_PIN_x vêm da HAL STM32F4. Não incluir este arquivo em core/ui.
 * Pinos do planejamento original mantidos; MAX31855 substitui MAX31865.
 */
#include "stm32f4xx_hal.h"

/* Segurança: PE0=RELAY_EN ativo alto; PE1=CUR_SEL alto=medição. */
#define PIN_RELAY_PORT GPIOE
#define PIN_RELAY       GPIO_PIN_0
#define PIN_SELECT_PORT GPIOE
#define PIN_SELECT       GPIO_PIN_1
#define PIN_RELAY_OFF    GPIO_PIN_RESET
#define PIN_RELAY_ON     GPIO_PIN_SET
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
