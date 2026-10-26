#include <assert.h>
#include <stdio.h>
#include "sd_io.h"
#include "hw_debug.h"
#include "runtime_config.h"
#include "stm32f4xx_hal.h"
SD_HandleTypeDef hsd;
static bool card;
static bool wide_ok=true;
void HAL_Delay(uint32_t ms) { (void)ms; }
static unsigned calls,resets,deinits;
void mock_sd_reset(bool asserted){if(asserted)resets++;}
HAL_StatusTypeDef HAL_SD_DeInit(SD_HandleTypeDef *s){assert(s==&hsd);deinits++;return HAL_OK;}
static uint32_t now;
uint32_t HAL_GetTick(void) { return now; }
uint32_t HAL_SD_GetCardState(SD_HandleTypeDef *s)
{ assert(s==&hsd);return card?HAL_SD_CARD_TRANSFER:HAL_SD_CARD_ERROR; }
HAL_StatusTypeDef HAL_SD_Init(SD_HandleTypeDef *s)
{
    assert(s==&hsd && s->Instance==SDIO && s->Init.BusWide==SDIO_BUS_WIDE_1B);
    calls++;return card?HAL_OK:HAL_ERROR;
}
HAL_StatusTypeDef HAL_SD_ConfigWideBusOperation(SD_HandleTypeDef *s,uint32_t bus)
{ assert(s==&hsd && bus==SDIO_BUS_WIDE_4B);calls++;return wide_ok?HAL_OK:HAL_ERROR; }
int main(void)
{
    assert(!board_sd_prepare() && !board_sd_is_ready() && calls==1);
    card=true;assert(board_sd_prepare() && board_sd_is_ready());
    assert(resets==2 && deinits==1);
#if THERMAL_SD_BUS_BITS == 4U
    assert(calls==3 && g_hw_debug.sd_bus_bits==4);
#else
    assert(calls==2 && g_hw_debug.sd_bus_bits==1);
#endif
#if THERMAL_SD_BUS_BITS == 4U
    wide_ok=false;assert(!board_sd_prepare() && g_hw_debug.sd_phase==4);
#endif
    wide_ok=true;assert(board_sd_prepare());
    card=false;now=300;assert(!board_sd_is_ready());
    puts("SD init without trapping on absent card: PASS");return 0;
}
