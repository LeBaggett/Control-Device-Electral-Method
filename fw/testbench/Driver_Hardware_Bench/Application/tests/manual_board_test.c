#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "board.h"
#include "pinout.h"
#include "power.h"
#include "dwt_us.h"
GPIO_TypeDef gpio_a = {0}, gpio_e = {.id=4}, gpio_d = {.id=3};
SPI_HandleTypeDef hspi1;
static unsigned relay, select_level, cs, clocks, transfers, gpio_inits;
static uint32_t now, pclk = 84000000U;
static bool timing_ok = true, init_ok = true;
static HAL_StatusTypeDef io_status = HAL_OK;
static char order[64]; static unsigned order_n;
static void record(char c) { order[order_n++] = c; order[order_n] = 0; }
void mock_clock(unsigned id) { clocks |= 1U << id; }
void HAL_Init(void) { record('H'); }
void HAL_GPIO_WritePin(GPIO_TypeDef *port, uint16_t pin, GPIO_PinState level)
{
    assert(clocks & (1U << port->id));
    if (port == PIN_SELECT_PORT && pin == PIN_SELECT) { select_level = level; }
    if (port == PIN_RELAY_PORT && pin == PIN_RELAY) {
        assert(BOARD_POWER_CONTROL_ENABLED || level == PIN_RELAY_OFF); relay = level;
    }
    if (port == GPIOA && pin == PIN_TC_CS) cs = level;
}
void HAL_GPIO_Init(GPIO_TypeDef *port, GPIO_InitTypeDef *io)
{
    (void)port; (void)io;
    assert(select_level == 1 && relay == 0 && cs == 1);
    gpio_inits++; if (gpio_inits == 3) record('S');
}
void board_cube_clock_init(void) { record('C'); }
void board_cube_gpio_init(void) { record('G'); }
void board_cube_peripherals_init(void) { record('P'); }
uint32_t HAL_GetTick(void) { return now; }
uint32_t HAL_RCC_GetPCLK2Freq(void) { return pclk; }
bool DWT_US_Init(void) { record('D'); return init_ok; }
bool DWT_US_Delay(uint32_t us) { assert(us == 1 || us == 1001); return timing_ok; }
HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *spi, uint8_t *tx,
                                          uint8_t *rx, uint16_t size, uint32_t timeout)
{
    assert(spi == &hspi1 && size == 4 && timeout == 10 && cs == 0);
    assert(tx[0] == 0 && tx[1] == 0 && tx[2] == 0 && tx[3] == 0);
    rx[0] = 0x01; rx[1] = 0x90; rx[2] = 0x12; rx[3] = 0x30;
    transfers++; return io_status;
}
int main(void)
{
    hspi1.Instance = SPI1;
    hspi1.Init = (SPI_InitTypeDef){SPI_MODE_MASTER, SPI_DIRECTION_2LINES,
        SPI_DATASIZE_8BIT, SPI_POLARITY_LOW, SPI_PHASE_1EDGE, SPI_NSS_SOFT,
        SPI_FIRSTBIT_MSB, SPI_TIMODE_DISABLE, SPI_CRCCALCULATION_DISABLE, 32U};
    /* HAL prescaler encoding 32 -> BR4 -> /32 -> 2.625 MHz. */
    board_init(); assert(board_is_ready());
    assert(strcmp(order, "HSCGDP") == 0);
    assert(board_power_control_enabled() == (BOARD_POWER_CONTROL_ENABLED != 0));
    assert(relay == 0 && select_level == 1); /* Never auto-start at boot. */
    assert(board_power_select_heating() && relay==0 && select_level==0);
    assert(board_power_relay_on() && relay==1 && select_level==0);
    board_power_relay_off();assert(relay==0 && select_level==0);
    board_power_select_measurement();assert(relay==0 && select_level==1);
    assert(board_power_relay_on() && board_power_select_heating());
    uint32_t raw = 123;
    now = 199; assert(!board_max31855_read32(&raw) && raw == 0 && transfers == 0);
    now = 200; assert(board_max31855_read32(&raw) && raw == 0x01901230U && cs == 1);
    now = 299; assert(!board_max31855_read32(&raw) && transfers == 1);
    now = 300; io_status = HAL_TIMEOUT;
    assert(!board_max31855_read32(&raw) && raw == 0 && cs == 1 && relay == 1 && select_level == 0);
    now = 400; io_status = HAL_OK; pclk = 168000000U;
    assert(!board_max31855_read32(&raw) && transfers == 2); /* >5 MHz */
    pclk = 84000000U;
    timing_ok=false;now=500;
    assert(!board_max31855_read32(&raw) && cs==1);
    assert(relay==1 && select_level==0); /* SPI/DWT fault does not switch manual IOs. */
    board_force_safe();assert(relay==0 && select_level==1);
    timing_ok=true;init_ok=false;order_n=0;
    board_init();assert(board_is_ready() && strchr(order,'P')!=NULL);
    puts("manual board GPIO boot and driver-fault isolation: PASS");
    return 0;
}
