#include "sd_io.h"
#include "hw_debug.h"
#include "runtime_config.h"
#include "stm32f4xx_hal.h"
/* MSP from CubeMX owns SDIO pin AF and clocks; pinout.h remains reference.
 * This initializer returns an absent-card error, never Error_Handler. */
extern SD_HandleTypeDef hsd;
volatile sd_io_debug_t g_sd_io_debug;
static bool ready;
static uint32_t last_probe;
bool board_sd_is_ready(void)
{
    uint32_t now = HAL_GetTick();
    if (ready && (uint32_t)(now-last_probe) >= 250U) {
        last_probe = now;
        g_hw_debug.sd_card_state = HAL_SD_GetCardState(&hsd);
        g_hw_debug.sd_hal_error = hsd.ErrorCode;
        if (g_hw_debug.sd_card_state != HAL_SD_CARD_TRANSFER) {
            ready = false; g_hw_debug.sd_phase = 6U;
        }
    }
    return ready;
}
bool board_sd_prepare(void)
{
    ready = false;
    g_sd_io_debug.sector_trace_count = 0;
    g_hw_debug.sd_attempts++;
    g_hw_debug.sd_phase = 1U;
    g_hw_debug.sd_mount_result = -1;
    g_hw_debug.sd_bus_bits = 1U;
    g_hw_debug.sd_wide_status = 0xFFFFFFFFU;
    HAL_Delay(THERMAL_SD_POWERUP_MS);
    /* Each explicit bench attempt starts with clean HAL/FIFO/peripheral state.
     * Does not power-cycle the card or touch relay/SELECT GPIOs. */
    if (hsd.Instance == SDIO) (void)HAL_SD_DeInit(&hsd);
    __HAL_RCC_SDIO_FORCE_RESET();
    __HAL_RCC_SDIO_RELEASE_RESET();
    hsd = (SD_HandleTypeDef){0};
    hsd.Instance = SDIO;
    hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
    hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
    hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
    hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
    hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
    hsd.Init.ClockDiv = THERMAL_SD_CLOCK_DIV; /* HAL uses its slow identification clock first. */
    HAL_StatusTypeDef result = HAL_SD_Init(&hsd);
    g_hw_debug.sd_init_status = result;
    g_hw_debug.sd_hal_error = hsd.ErrorCode;
    if (result != HAL_OK) { g_hw_debug.sd_phase = 2U; return false; }
#if THERMAL_SD_BUS_BITS == 4U
    g_hw_debug.sd_phase = 3U;
    result = HAL_SD_ConfigWideBusOperation(&hsd,SDIO_BUS_WIDE_4B);
    g_hw_debug.sd_wide_status = result;
    g_hw_debug.sd_hal_error = hsd.ErrorCode;
    if (result != HAL_OK) { g_hw_debug.sd_phase = 4U; return false; }
    g_hw_debug.sd_bus_bits = 4U;
#elif THERMAL_SD_BUS_BITS != 1U
#error "THERMAL_SD_BUS_BITS must be 1 or 4"
#endif
    g_sd_io_debug.card_type = hsd.SdCard.CardType;
    g_sd_io_debug.card_logical_blocks = hsd.SdCard.LogBlockNbr;
    g_hw_debug.sd_phase = 5U;
    last_probe = HAL_GetTick(); ready = true; return true;
}
