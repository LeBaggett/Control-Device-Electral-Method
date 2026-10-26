#include "board.h"
#include "pinout.h"
#include "main.h"
#include "bench_diag.h"
void bench_board_leds(uint32_t green,uint32_t yellow,uint32_t red) {
 HAL_GPIO_WritePin(BENCH_LED_GREEN_PORT,BENCH_LED_GREEN_PIN,green?GPIO_PIN_SET:GPIO_PIN_RESET);
 HAL_GPIO_WritePin(BENCH_LED_YELLOW_PORT,BENCH_LED_YELLOW_PIN,yellow?GPIO_PIN_SET:GPIO_PIN_RESET);
 HAL_GPIO_WritePin(BENCH_LED_RED_PORT,BENCH_LED_RED_PIN,red?GPIO_PIN_SET:GPIO_PIN_RESET);
}
static void clock_init(void) {
 RCC_OscInitTypeDef osc={0};RCC_ClkInitTypeDef clk={0};
 if(HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY)!=HAL_OK)bench_fatal(1);
 __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
 uint32_t start=HAL_GetTick();
 while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))if(HAL_GetTick()-start>=100U)bench_fatal(2);
 /* HSI avoids relying on the ST-LINK MCO cable/8 MHz HSE bridge setup.
  * 64/4*50/2 = 400 MHz core; HCLK 200 MHz. This is bring-up, not final metrology. */
 osc.OscillatorType=RCC_OSCILLATORTYPE_HSI;osc.HSIState=RCC_HSI_DIV1;
 osc.HSICalibrationValue=RCC_HSICALIBRATION_DEFAULT;
 osc.PLL.PLLState=RCC_PLL_ON;osc.PLL.PLLSource=RCC_PLLSOURCE_HSI;
 osc.PLL.PLLM=4;osc.PLL.PLLN=50;osc.PLL.PLLP=2;osc.PLL.PLLQ=4;osc.PLL.PLLR=2;
 osc.PLL.PLLRGE=RCC_PLL1VCIRANGE_3;osc.PLL.PLLVCOSEL=RCC_PLL1VCOWIDE;
 if(HAL_RCC_OscConfig(&osc)!=HAL_OK)bench_fatal(3);
 clk.ClockType=RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_D1PCLK1|
 RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2|RCC_CLOCKTYPE_D3PCLK1;
 clk.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;clk.SYSCLKDivider=RCC_SYSCLK_DIV1;
 clk.AHBCLKDivider=RCC_HCLK_DIV2;clk.APB3CLKDivider=RCC_APB3_DIV2;
 clk.APB1CLKDivider=RCC_APB1_DIV2;clk.APB2CLKDivider=RCC_APB2_DIV2;clk.APB4CLKDivider=RCC_APB4_DIV2;
 if(HAL_RCC_ClockConfig(&clk,FLASH_LATENCY_4)!=HAL_OK)bench_fatal(4);
 SystemCoreClockUpdate();g_bench_debug.core_hz=SystemCoreClock;g_bench_debug.hclk_hz=HAL_RCC_GetHCLKFreq();
}
void bench_board_init(void) {
 __HAL_RCC_GPIOB_CLK_ENABLE();__HAL_RCC_GPIOE_CLK_ENABLE();
 bench_board_leds(0,0,0);GPIO_InitTypeDef gpio={0};
 gpio.Mode=GPIO_MODE_OUTPUT_PP;gpio.Pull=GPIO_NOPULL;gpio.Speed=GPIO_SPEED_FREQ_LOW;
 gpio.Pin=BENCH_LED_GREEN_PIN|BENCH_LED_RED_PIN;HAL_GPIO_Init(GPIOB,&gpio);
 gpio.Pin=BENCH_LED_YELLOW_PIN;HAL_GPIO_Init(GPIOE,&gpio);
 clock_init();
 __HAL_RCC_D2SRAM1_CLK_ENABLE();__HAL_RCC_D2SRAM2_CLK_ENABLE();__HAL_RCC_D2SRAM3_CLK_ENABLE();
}
