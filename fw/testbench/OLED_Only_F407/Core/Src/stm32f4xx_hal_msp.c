#include "main.h"
void HAL_MspInit(void) { __HAL_RCC_SYSCFG_CLK_ENABLE(); __HAL_RCC_PWR_CLK_ENABLE(); }
/* Pin setup is explicit in oled_probe.c, after the peripheral clock is on. */
void HAL_I2C_MspInit(I2C_HandleTypeDef *h) { (void)h; }
void HAL_I2C_MspDeInit(I2C_HandleTypeDef *h) { (void)h; }
