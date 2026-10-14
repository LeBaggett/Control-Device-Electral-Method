#include "peripheral_io.h"
#include "board.h"
#include "pinout.h"
#include "dwt_us.h"
extern I2C_HandleTypeDef hi2c1;
extern TIM_HandleTypeDef htim2;
extern ADC_HandleTypeDef hadc1;
bool board_oled_write(const uint8_t *bytes, size_t count)
{
    if (!board_is_ready() || !bytes || count == 0 || count > 32U) return false;
    return HAL_I2C_Master_Transmit(&hi2c1, OLED_I2C_ADDRESS_7BIT << 1,
        (uint8_t *)bytes, (uint16_t)count, 5U) == HAL_OK;
}
bool board_encoder_start(uint32_t *counter)
{
    if (!board_is_ready() || !counter || htim2.Instance != TIM2 ||
        htim2.Init.Period != UINT32_MAX ||
        HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL) != HAL_OK) return false;
    *counter = __HAL_TIM_GET_COUNTER(&htim2);
    return true;
}
uint32_t board_encoder_counter(void) { return __HAL_TIM_GET_COUNTER(&htim2); }
bool board_encoder_button_pressed(void)
{
    return HAL_GPIO_ReadPin(PIN_ENC_SW_PORT, PIN_ENC_SW) == GPIO_PIN_RESET;
}
bool board_adc_start(void)
{
    if (!board_is_ready() || hadc1.Instance != ADC1 ||
        hadc1.Init.Resolution != ADC_RESOLUTION_12B ||
        hadc1.Init.DataAlign != ADC_DATAALIGN_RIGHT ||
        hadc1.Init.ScanConvMode != DISABLE ||
        hadc1.Init.NbrOfConversion != 1U ||
        hadc1.Init.ContinuousConvMode != DISABLE ||
        hadc1.Init.ExternalTrigConvEdge != ADC_EXTERNALTRIGCONVEDGE_NONE ||
        hadc1.Init.EOCSelection != ADC_EOC_SINGLE_CONV ||
        hadc1.Init.ClockPrescaler != ADC_CLOCK_SYNC_PCLK_DIV4 ||
        HAL_RCC_GetPCLK2Freq()/4U > 36000000U)
        return false;
    ADC->CCR |= ADC_CCR_TSVREFE;
    return DWT_US_Delay(10U); /* VREFINT startup before first conversion. */
}
bool board_adc_begin(bool vrefint)
{
    ADC_ChannelConfTypeDef channel = {0};
    channel.Channel = vrefint ? ADC_CHANNEL_VREFINT : PIN_VF_ADC_CHANNEL;
    channel.Rank = 1U;
    channel.SamplingTime = vrefint ? ADC_SAMPLETIME_480CYCLES : ADC_SAMPLETIME_15CYCLES;
    return HAL_ADC_ConfigChannel(&hadc1, &channel) == HAL_OK &&
           HAL_ADC_Start(&hadc1) == HAL_OK;
}
void board_adc_cancel(void) { (void)HAL_ADC_Stop(&hadc1); }
uint16_t board_vrefint_calibration(void)
{
    /* DS8626 Rev12 table73, validate content before using it. */
    return *(const uint16_t *)(uintptr_t)0x1FFF7A2AU;
}
bool board_adc_sample(uint16_t *value)
{
    if (!value || HAL_ADC_PollForConversion(&hadc1, 0U) != HAL_OK) return false;
    uint32_t raw = HAL_ADC_GetValue(&hadc1);
    if (raw > 4095U) return false;
    *value = (uint16_t)raw;
    (void)HAL_ADC_Stop(&hadc1);
    return true;
}
bool board_adc_burst(unsigned count, float *average)
{
    if (!average || count == 0 || count > 64 || !DWT_US_Delay(0)) return false;
    uint32_t start = DWT->CYCCNT;
    uint32_t limit = SystemCoreClock / 2500U; /* Entire burst deadline: 400 us. */
    uint32_t guard = 100000U, sum = 0;
    board_adc_cancel();
    for (unsigned i = 0; i < count; ++i) {
        if (!board_adc_begin(false)) { board_adc_cancel(); return false; }
        uint16_t raw;
        while (!board_adc_sample(&raw)) {
            if (--guard == 0 || (uint32_t)(DWT->CYCCNT-start) >= limit) {
                board_adc_cancel(); return false;
            }
        }
        if (raw == 0 || raw >= 4095 || (uint32_t)(DWT->CYCCNT-start) >= limit) {
            board_adc_cancel(); return false;
        }
        sum += raw;
    }
    *average = (float)sum / count; return true;
}
