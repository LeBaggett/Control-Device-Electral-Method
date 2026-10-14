#include <assert.h>
#include <stdio.h>
#include "sd_io.h"
#include "stm32f4xx_hal.h"
SD_HandleTypeDef hsd;
static bool card;
static unsigned calls;
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
{ assert(s==&hsd && bus==SDIO_BUS_WIDE_4B);calls++;return HAL_OK; }
int main(void)
{
    assert(!board_sd_prepare() && !board_sd_is_ready() && calls==1);
    card=true;assert(board_sd_prepare() && board_sd_is_ready() && calls==3);
    card=false;now=300;assert(!board_sd_is_ready());
    puts("SD init without trapping on absent card: PASS");return 0;
}
