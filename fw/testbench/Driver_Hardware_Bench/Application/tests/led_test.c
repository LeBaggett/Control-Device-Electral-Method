#include <assert.h>
#include <stdio.h>
#include "bench_led.h"
#include "pinout.h"
GPIO_TypeDef gpio_a={0},gpio_b={1},gpio_c={2},gpio_d={3},gpio_e={4},gpio_h={7};
uint32_t SystemCoreClock=168000000U;
static unsigned clock_ready,writes,inits;
static GPIO_PinState last_level;
void mock_clock(unsigned id) { assert(id==0);clock_ready=1; }
void HAL_GPIO_WritePin(GPIO_TypeDef *p,uint16_t pin,GPIO_PinState value)
{
    assert(clock_ready && p==GPIOA && pin==GPIO_PIN_1);
    writes++;last_level=value;
}
void HAL_GPIO_Init(GPIO_TypeDef *p,GPIO_InitTypeDef *io)
{
    assert(p==GPIOA && io->Pin==GPIO_PIN_1 && io->Mode==GPIO_MODE_OUTPUT_PP);
    assert(writes==1 && last_level==GPIO_PIN_SET); /* Off preloaded before output. */
    inits++;
}
int main(void)
{
    board_bench_led_write(true);assert(writes==0);
    assert(BENCH_LED_CONFIGURED==1 && BENCH_LED_ACTIVE_LEVEL==GPIO_PIN_RESET);
    assert(board_bench_led_init() && inits==1 && last_level==GPIO_PIN_SET);
    board_bench_led_write(true);assert(last_level==GPIO_PIN_RESET);
    board_bench_led_write(false);assert(last_level==GPIO_PIN_SET);
    puts("PA1 active-low LED: safe preload and ON/OFF polarity PASS");return 0;
}
