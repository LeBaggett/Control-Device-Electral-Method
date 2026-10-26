#include <assert.h>
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "dwt_us.h"
DWT_Type mock_dwt;
CoreDebug_Type mock_debug;
uint32_t SystemCoreClock = 168000000U;
static unsigned running = 1;
void SystemCoreClockUpdate(void) { }
void mock_nop(void) { if (running) mock_dwt.CYCCNT++; }
int main(void)
{
    assert(!DWT_US_Delay(1));
    assert(DWT_US_Init());
    assert(DWT_US_Delay(0));
    assert(!DWT_US_Delay(1001));
    SystemCoreClock = 84000000U;
    assert(!DWT_US_Delay(0));
    assert(DWT_US_Init());
    /* Host counter stationary in wait loop: guard must stop the wait. */
    assert(!DWT_US_Delay(1));
    running = 0; assert(!DWT_US_Init());
    SystemCoreClock = 0; assert(!DWT_US_Init());
    puts("DWT failure/clock guard tests: PASS");
    return 0;
}
