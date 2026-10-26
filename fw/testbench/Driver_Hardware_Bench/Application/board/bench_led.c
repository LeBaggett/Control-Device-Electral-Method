#include "bench_led.h"
#include "pinout.h"
#include "bench_config.h"
static bool led_initialized;
uint32_t board_bench_core_clock(void) { return SystemCoreClock; }
bool board_bench_led_init(void)
{
    led_initialized = false;
#if BENCH_LED_CONFIGURED
    struct reserved_pin { GPIO_TypeDef *port; uint16_t mask; };
    const struct reserved_pin reserved[] = {
        {PIN_RELAY_PORT,PIN_RELAY}, {PIN_SELECT_PORT,PIN_SELECT}, {PIN_ENC_SW_PORT,PIN_ENC_SW},
        {GPIOA, PIN_TC_CS|GPIO_PIN_13|GPIO_PIN_14},
        {GPIOH, GPIO_PIN_0|GPIO_PIN_1},
#if BENCH_TEST_MAX31855
        {GPIOA, PIN_TC_SCK|PIN_TC_MISO|PIN_TC_MOSI_RESERVED},
#endif
#if BENCH_TEST_ENCODER
        {PIN_ENC_A_PORT, PIN_ENC_A}, {PIN_ENC_B_PORT, PIN_ENC_B},
#endif
#if BENCH_TEST_OLED
        {GPIOB, PIN_OLED_SCL|PIN_OLED_SDA},
#endif
#if BENCH_TEST_ADC
        {PIN_VF_PORT,PIN_VF},
#endif
#if BENCH_TEST_SD
        {GPIOC, PIN_SD_D0|PIN_SD_D1|PIN_SD_D2|PIN_SD_D3|PIN_SD_CK},
        {PIN_SD_CMD_PORT,PIN_SD_CMD},
#endif
    };
    for (unsigned i=0;i<sizeof(reserved)/sizeof(reserved[0]);++i)
        if (BENCH_LED_PORT == reserved[i].port && (BENCH_LED_PIN & reserved[i].mask))
            return false;
    if (BENCH_LED_PIN == 0 || (BENCH_LED_PIN & (BENCH_LED_PIN-1U))) return false;
    BENCH_LED_CLOCK_ENABLE();
    HAL_GPIO_WritePin(BENCH_LED_PORT, BENCH_LED_PIN,
                     BENCH_LED_ACTIVE_LEVEL == GPIO_PIN_SET ? GPIO_PIN_RESET : GPIO_PIN_SET);
    GPIO_InitTypeDef io = {0};
    io.Pin = BENCH_LED_PIN; io.Mode = GPIO_MODE_OUTPUT_PP;
    io.Pull = GPIO_NOPULL; io.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BENCH_LED_PORT,&io);
    led_initialized = true;
    return true;
#else
    return false; /* No guessed GPIO while the board LED pin is unknown. */
#endif
}
void board_bench_led_write(bool on)
{
    if (!led_initialized) return;
#if BENCH_LED_CONFIGURED
    HAL_GPIO_WritePin(BENCH_LED_PORT,BENCH_LED_PIN,on ? BENCH_LED_ACTIVE_LEVEL :
        (BENCH_LED_ACTIVE_LEVEL == GPIO_PIN_SET ? GPIO_PIN_RESET : GPIO_PIN_SET));
#else
    (void)on;
#endif
}
