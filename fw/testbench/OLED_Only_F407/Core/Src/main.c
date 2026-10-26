#include "main.h"
#include "pinout.h"
#include "oled_probe.h"
static void safe_gpio(void) {
 __HAL_RCC_GPIOD_CLK_ENABLE(); __HAL_RCC_GPIOA_CLK_ENABLE();
 HAL_GPIO_WritePin(SELECT_PORT,SELECT_PIN,GPIO_PIN_SET);
 HAL_GPIO_WritePin(RELAY_PORT,RELAY_PIN,GPIO_PIN_RESET);
 GPIO_InitTypeDef p={0}; p.Mode=GPIO_MODE_OUTPUT_PP;p.Pull=GPIO_NOPULL;p.Speed=GPIO_SPEED_FREQ_LOW;
 p.Pin=SELECT_PIN|RELAY_PIN;HAL_GPIO_Init(GPIOD,&p);
 HAL_GPIO_WritePin(LED_PORT,LED_PIN,GPIO_PIN_SET);
 p.Pin=LED_PIN;HAL_GPIO_Init(LED_PORT,&p);
}
static void clock_hsi(void) {
 RCC_OscInitTypeDef o={0};RCC_ClkInitTypeDef c={0};
 o.OscillatorType=RCC_OSCILLATORTYPE_HSI;o.HSIState=RCC_HSI_ON;
 o.HSICalibrationValue=RCC_HSICALIBRATION_DEFAULT;o.PLL.PLLState=RCC_PLL_OFF;
 if(HAL_RCC_OscConfig(&o)!=HAL_OK)Error_Handler();
 c.ClockType=RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
 c.SYSCLKSource=RCC_SYSCLKSOURCE_HSI;c.AHBCLKDivider=RCC_SYSCLK_DIV1;
 c.APB1CLKDivider=RCC_HCLK_DIV1;c.APB2CLKDivider=RCC_HCLK_DIV1;
 if(HAL_RCC_ClockConfig(&c,FLASH_LATENCY_0)!=HAL_OK)Error_Handler();
}
int main(void) {
 HAL_Init();safe_gpio();clock_hsi();HAL_Delay(500);
 oled_test_run();
 for(;;) {
  g_oled_test.heartbeat++;
  HAL_GPIO_TogglePin(LED_PORT,LED_PIN);
  if(g_oled_test.retry){g_oled_test.retry=0;oled_test_run();}
  HAL_Delay(250);
 }
}
void Error_Handler(void) {
 g_oled_test.failure=9;g_oled_test.phase=9;
 for(;;){HAL_GPIO_TogglePin(LED_PORT,LED_PIN);HAL_Delay(100);}
}
