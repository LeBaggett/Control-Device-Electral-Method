#include "board.h"
#include "pinout.h"
#include "dwt_us.h"
#include <stddef.h>

/* Wrappers in generated main.c USER CODE 4; see integration instructions. */
extern void board_cube_clock_init(void);
extern void board_cube_gpio_init(void);
extern void board_cube_peripherals_init(void);
extern SPI_HandleTypeDef hspi1;
static bool s_board_ready;
static bool s_relay_commanded_on;
static bool s_select_measurement;
static uint32_t s_last_tc_ms, s_tc_interval_ms;

void board_power_select_measurement(void)
{
    HAL_GPIO_WritePin(PIN_SELECT_PORT, PIN_SELECT, PIN_SELECT_MEAS);
    s_select_measurement = true;
}
void board_power_relay_off(void)
{
    HAL_GPIO_WritePin(PIN_RELAY_PORT, PIN_RELAY, PIN_RELAY_OFF);
    s_relay_commanded_on = false;
}
void board_force_safe(void)
{
    /* Preserve v4 mandatory order: measurement BEFORE relay off. */
    board_power_select_measurement();
    board_power_relay_off();
    HAL_GPIO_WritePin(PIN_TC_CS_PORT, PIN_TC_CS, GPIO_PIN_SET);
}
bool board_power_select_heating(void)
{
    if (!BOARD_POWER_CONTROL_ENABLED) return false;
    HAL_GPIO_WritePin(PIN_SELECT_PORT, PIN_SELECT, PIN_SELECT_HEAT);
    s_select_measurement = false;
    return true;
}
bool board_power_relay_on(void)
{
    if (!BOARD_POWER_CONTROL_ENABLED) return false;
    HAL_GPIO_WritePin(PIN_RELAY_PORT, PIN_RELAY, PIN_RELAY_ON);
    s_relay_commanded_on = true;
    return true;
}
bool board_power_control_enabled(void) { return BOARD_POWER_CONTROL_ENABLED != 0; }
static void board_safe_gpio_early(void)
{
    GPIO_InitTypeDef io = {0};
    PIN_SELECT_CLOCK_ENABLE();
    PIN_RELAY_CLOCK_ENABLE();
    PIN_TC_CS_CLOCK_ENABLE();
    board_force_safe(); /* Preload output latches BEFORE output mode. */
    io.Mode = GPIO_MODE_OUTPUT_PP;
    io.Pull = GPIO_NOPULL;
    io.Speed = GPIO_SPEED_FREQ_LOW;
    io.Pin = PIN_SELECT;
    io.Pull = PIN_SELECT_PULL;
    HAL_GPIO_Init(PIN_SELECT_PORT, &io);
    io.Pin = PIN_RELAY;
    io.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(PIN_RELAY_PORT, &io);
    io.Pin = PIN_TC_CS;
    HAL_GPIO_Init(PIN_TC_CS_PORT, &io);
}
void board_init(void)
{
    s_board_ready = false;
    HAL_Init(); /* Same ordered initialization contract as v4. */
    board_safe_gpio_early();
    board_cube_clock_init();
    board_cube_gpio_init(); /* Generated initial states MUST match pinout.h. */
    board_force_safe();
    (void)DWT_US_Init(); /* One unavailable driver must not block OLED/other tests. */
    board_cube_peripherals_init(); /* SPI/ADC/I2C/encoder + FatFs registration; no SD probe. */
    board_force_safe();
    s_last_tc_ms = HAL_GetTick();
    s_tc_interval_ms = 200U; /* Assumes sensor supply already stable here. */
    s_board_ready = true;
}
bool board_is_ready(void) { return s_board_ready; }
uint32_t board_millis(void) { return HAL_GetTick(); }
void board_delay_us(uint32_t us)
{
    /* Legacy interface retained. On failure, disable board reads as well. */
    (void)DWT_US_Delay(us); /* Timing validation does not control manual IOs. */
}
static bool read_fail(void)
{
    HAL_GPIO_WritePin(PIN_TC_CS_PORT, PIN_TC_CS, GPIO_PIN_SET);
    return false; /* Only release this driver's CS; never change manual outputs. */
}
bool board_max31855_read32(uint32_t *raw)
{
    uint8_t tx[4] = {0}, rx[4] = {0};
    if (raw != NULL) *raw = 0U;
    if (!s_board_ready || raw == NULL) return read_fail();
    if ((uint32_t)(HAL_GetTick() - s_last_tc_ms) < s_tc_interval_ms)
        return false;
    if (hspi1.Instance != SPI1 || hspi1.Init.Mode != SPI_MODE_MASTER ||
        hspi1.Init.Direction != SPI_DIRECTION_2LINES ||
        hspi1.Init.DataSize != SPI_DATASIZE_8BIT ||
        hspi1.Init.CLKPolarity != SPI_POLARITY_LOW ||
        hspi1.Init.CLKPhase != SPI_PHASE_1EDGE ||
        hspi1.Init.NSS != SPI_NSS_SOFT ||
        hspi1.Init.FirstBit != SPI_FIRSTBIT_MSB ||
        hspi1.Init.TIMode != SPI_TIMODE_DISABLE ||
        hspi1.Init.CRCCalculation != SPI_CRCCALCULATION_DISABLE)
        return read_fail();
    uint32_t br = hspi1.Init.BaudRatePrescaler >> 3U;
    if (br > 7U || HAL_RCC_GetPCLK2Freq() > 5000000U * (2U << br))
        return read_fail();
    HAL_GPIO_WritePin(PIN_TC_CS_PORT, PIN_TC_CS, GPIO_PIN_RESET);
    if (!DWT_US_Delay(1U)) {
        return read_fail();
    }
    HAL_StatusTypeDef result = HAL_SPI_TransmitReceive(&hspi1, tx, rx, 4U, 10U);
    bool hold_ok = DWT_US_Delay(1U);
    HAL_GPIO_WritePin(PIN_TC_CS_PORT, PIN_TC_CS, GPIO_PIN_SET);
    s_last_tc_ms = HAL_GetTick();
    s_tc_interval_ms = 100U;
    if (result != HAL_OK || !hold_ok) return read_fail();
    *raw = ((uint32_t)rx[0] << 24U) | ((uint32_t)rx[1] << 16U) |
           ((uint32_t)rx[2] << 8U) | (uint32_t)rx[3];
    return true;
}
