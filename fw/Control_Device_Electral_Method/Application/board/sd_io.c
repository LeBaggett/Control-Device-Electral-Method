#include "sd_io.h"
#include "stm32f4xx_hal.h"
/* MSP from CubeMX owns SDIO pin AF and clocks; pinout.h remains reference.
 * This initializer returns an absent-card error, never Error_Handler. */
extern SD_HandleTypeDef hsd;
static bool ready;
static uint32_t last_probe;
bool board_sd_is_ready(void)
{
    uint32_t now = HAL_GetTick();
    if (ready && (uint32_t)(now-last_probe) >= 250U) {
        last_probe = now;
        if (HAL_SD_GetCardState(&hsd) != HAL_SD_CARD_TRANSFER) ready = false;
    }
    return ready;
}
bool board_sd_prepare(void)
{
    ready = false;
    hsd.Instance = SDIO;
    hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
    hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
    hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
    hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
    hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
    hsd.Init.ClockDiv = 2U; /* 48MHz/(2+2) = 12MHz transfer; init uses HAL slow clock. */
    if (HAL_SD_Init(&hsd) != HAL_OK) return false;
    if (HAL_SD_ConfigWideBusOperation(&hsd,SDIO_BUS_WIDE_4B) != HAL_OK) return false;
    last_probe = HAL_GetTick(); ready = true; return true;
}
