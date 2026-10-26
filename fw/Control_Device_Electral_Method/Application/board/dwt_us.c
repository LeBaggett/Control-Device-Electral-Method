#include "dwt_us.h"
#include "stm32f4xx_hal.h"
static uint32_t core_hz;

bool DWT_US_Init(void)
{
    core_hz = 0;
    SystemCoreClockUpdate();
    if (SystemCoreClock < 1000000U || SystemCoreClock > 168000000U)
        return false;
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    __DSB();
    __ISB();
    uint32_t before = DWT->CYCCNT;
    for (volatile uint32_t i = 0; i < 64U; ++i) { __NOP(); }
    if (DWT->CYCCNT == before) return false;
    core_hz = SystemCoreClock;
    return true;
}

/* Diferenca sem sinal suporta wrap do contador para os atrasos curtos aceitos.
 * Se o clock mudar depois do init, recusar o atraso em vez de usar escala antiga. */
bool DWT_US_Delay(uint32_t us)
{
    if (!core_hz || core_hz != SystemCoreClock || us > 1000U ||
        !(DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk)) return false;
    uint32_t cycles = (uint32_t)(((uint64_t)core_hz * us + 999999U) / 1000000U);
    uint32_t start = DWT->CYCCNT;
    uint32_t budget = 1000000U; /* Prevent infinite wait if counter stops. */
    while ((uint32_t)(DWT->CYCCNT - start) < cycles) {
        if (--budget == 0U) return false;
    }
    return true; /* Unsigned subtraction tolerates one counter wrap. */
}
