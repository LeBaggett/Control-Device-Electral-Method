#include "sd_io.h"
#include "hw_debug.h"
#include "runtime_config.h"
#include "bsp_driver_sd.h"
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

/* Uses a dedicated aligned buffer, never the filesystem's sector buffer. */
volatile sd_read_diag_t g_sd_read_diag;
static uint32_t diag_buffer[128];
bool board_sd_read_diagnostic(void)
{
    g_sd_read_diag=(sd_read_diag_t){0};
    g_sd_read_diag.running=1U;
    if(!board_sd_prepare()) {g_sd_read_diag.running=0U;return false;}
    g_sd_read_diag.init_ok=1U;
    bool ok=true;
    for(unsigned i=0;i<6U;i++) {
        volatile sd_read_diag_sample_t *r=&g_sd_read_diag.samples[i];
        r->sector=i<3U?0U:32768U;
        uint8_t *bytes=(uint8_t *)diag_buffer;
        /* Per-byte/per-attempt pattern distinguishes untouched bytes from zero. */
        for(unsigned j=0;j<512U;j++)bytes[j]=(uint8_t)(0xA5U+j*37U+i*13U);
        uint32_t start=HAL_GetTick();
        r->status=BSP_SD_ReadBlocks(diag_buffer,r->sector,1U,500U);
        r->hal_error=hsd.ErrorCode;r->elapsed_ms=HAL_GetTick()-start;
        r->sta=hsd.Instance->STA;r->dcount=hsd.Instance->DCOUNT;
        r->dlen=hsd.Instance->DLEN;r->clkcr=hsd.Instance->CLKCR;
        r->response1=hsd.Instance->RESP1;
        r->card_state_initial=HAL_SD_GetCardState(&hsd);
        r->card_state=r->card_state_initial;
        uint32_t wait_start=HAL_GetTick();
        while(r->card_state!=HAL_SD_CARD_TRANSFER &&
              (uint32_t)(HAL_GetTick()-wait_start)<250U) {
            HAL_Delay(1U);
            r->card_state=HAL_SD_GetCardState(&hsd);
        }
        r->wait_ms=HAL_GetTick()-wait_start;
        r->wait_timeout=r->card_state!=HAL_SD_CARD_TRANSFER;
        r->state_hal_error=hsd.ErrorCode;
        r->state_response1=hsd.Instance->RESP1;
        uint32_t hash=2166136261U;
        for(unsigned j=0;j<512U;j++) {
            if(bytes[j]==0U)r->zero_bytes++;
            if(bytes[j]==(uint8_t)(0xA5U+j*37U+i*13U))r->unchanged_bytes++;
            hash=(hash^bytes[j])*16777619U;
        }
        r->hash=hash;r->signature=sd_le16(bytes+510);
        for(unsigned j=0;j<16U;j++)r->first_bytes[j]=bytes[j];
        r->matches_first=(i%3U)==0U?1U:(hash==g_sd_read_diag.samples[i-i%3U].hash);
        if(i==0U && r->status==HAL_OK) {
            g_sd_read_diag.partition_type=bytes[450];
            g_sd_read_diag.partition_lba=sd_le32(bytes+454);
        }
        if(r->status!=HAL_OK || r->signature!=0xAA55U || !r->matches_first)ok=false;
        g_sd_read_diag.completed=i+1U;
        /* Do not send a new read while the previous command is still active. */
        if(r->card_state!=HAL_SD_CARD_TRANSFER) {ok=false;break;}
        HAL_Delay(10U);
    }
    g_sd_read_diag.running=0U;
    return ok;
}
