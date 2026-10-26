#include "main.h"
#include "board.h"
#include "bench_diag.h"
#include "bench_config.h"
#include "net_service.h"
int main(void) {
 /* Stage 1 uses I-cache only. D-cache deliberately OFF to isolate DMA issues.
  * DMA descriptors/RX pool/heap are still explicitly placed in D2 SRAM.
  * Enabling D-cache requires its own cache/MPU validation in the next stage. */
 SCB_EnableICache();HAL_Init();bench_diag_init(HAL_GetTick());g_bench_debug.boot_stage=1;
 bench_board_init();g_bench_debug.boot_stage=3;bench_net_init();
 uint32_t led_ms=HAL_GetTick(),green=0;
 for(;;){
  bench_diag_step(HAL_GetTick());bench_net_step();uint32_t now=HAL_GetTick();
  if((uint32_t)(now-led_ms)>=BENCH_HEARTBEAT_MS){led_ms=now;green=!green;g_bench_debug.led_toggles++;}
  bench_board_leds(green,g_bench_debug.link_up,0);
 }
}
void bench_fatal(uint32_t code){
 g_bench_debug.fatal_code=code;g_bench_debug.boot_stage=99;
 g_bench_debug.cfsr=SCB->CFSR;g_bench_debug.hfsr=SCB->HFSR;
 g_bench_debug.bfar=SCB->BFAR;g_bench_debug.mmfar=SCB->MMFAR;
 __disable_irq();bench_board_leds(0,0,1);for(;;)__NOP();
}
void Error_Handler(void){bench_fatal(100);}
