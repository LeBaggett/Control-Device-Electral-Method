#include "peripheral_io.h"
#include "hw_debug.h"
#include "board.h"
#include "pinout.h"
#include "dwt_us.h"
#include "vf_adc.h"
extern I2C_HandleTypeDef hi2c1;
extern TIM_HandleTypeDef htim2;
extern ADC_HandleTypeDef hadc1;
uint8_t board_oled_controller(void) { return OLED_SELECTED_DRIVER; }
uint8_t board_oled_column_offset(void)
{
    return OLED_SELECTED_DRIVER == OLED_DRIVER_SH1106 ? OLED_SH1106_COLUMN_OFFSET : 0U;
}
bool board_oled_prepare(void)
{
    if (!board_is_ready()) return false;
    g_hw_debug.oled_prepare_attempts++;
    HAL_Delay(350U); /* Startup/recovery only; not part of the acquisition window. */
    HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(&hi2c1, OLED_I2C_ADDRESS_7BIT << 1, 3U, 10U);
    g_hw_debug.oled_last_hal_status = status;
    return status == HAL_OK;
}
bool board_oled_write(const uint8_t *bytes, size_t count)
{
    if (!board_is_ready() || !bytes || count == 0 || count > 32U) return false;
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(&hi2c1, OLED_I2C_ADDRESS_7BIT << 1,
        (uint8_t *)bytes, (uint16_t)count, 5U);
    g_hw_debug.oled_last_hal_status = status;
    if (status != HAL_OK) g_hw_debug.oled_write_errors++;
    return status == HAL_OK;
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
    g_vf_debug.completed_samples=0;g_vf_debug.elapsed_cycles=0;g_vf_debug.last_burst_raw=0;
    if (!average || count==0 || count>64) {g_vf_debug.burst_status=1;return false;}
    if (!DWT_US_Delay(0)) {g_vf_debug.burst_status=2;return false;}
    uint32_t start=DWT->CYCCNT;
    uint32_t limit=SystemCoreClock/2500U; /* Preserve 400 us deadline. */
    uint32_t guard=100000U,sum=0;
    board_adc_cancel();
    /* Configure/enable once. Repeated HAL Stop/Start includes ADC startup
     * delays and cannot acquire 64 samples inside 400 us in a Debug build.
     * Keep single-conversion mode; reading DR clears EOC, then SWSTART
     * triggers the next conversion without powering down the ADC. */
    if (!board_adc_begin(false)) {g_vf_debug.burst_status=3;board_adc_cancel();return false;}
    for(unsigned i=0;i<count;i++) {
        while((hadc1.Instance->SR & ADC_SR_EOC)==0U) {
            g_vf_debug.elapsed_cycles=(uint32_t)(DWT->CYCCNT-start);
            if(--guard==0 || g_vf_debug.elapsed_cycles>=limit) {
                g_vf_debug.burst_status=4;board_adc_cancel();return false;
            }
            __NOP();
        }
        uint32_t raw=HAL_ADC_GetValue(&hadc1);
        g_vf_debug.last_burst_raw=raw;
        g_vf_debug.elapsed_cycles=(uint32_t)(DWT->CYCCNT-start);
        if(raw==0 || raw>=4095 || g_vf_debug.elapsed_cycles>=limit) {
            g_vf_debug.burst_status=raw==0?5U:raw>=4095?6U:7U;
            board_adc_cancel();return false;
        }
        sum+=raw;g_vf_debug.completed_samples=i+1U;
        if(i+1U<count)hadc1.Instance->CR2 |= ADC_CR2_SWSTART;
    }
    board_adc_cancel();
    *average=(float)sum/count;g_vf_debug.burst_status=0;return true;
}

#include "hw_debug.h"
#include <math.h>
#include "oled.h"
#include "sdlog.h"
#include "vf_adc.h"
void board_debug_service(void)
{
 static uint32_t last_ms;
 uint32_t now = board_millis();
 if ((uint32_t)(now-last_ms) < 50U) return;
 last_ms = now;
 g_hw_debug.sequence++; /* Odd while snapshot is being updated. */
 g_hw_debug.updated_ms = now;
 g_hw_debug.board_ready = board_is_ready();
 g_hw_debug.power_enabled = board_power_control_enabled();
 g_hw_debug.gpio_a_idr = GPIOA->IDR; g_hw_debug.gpio_a_odr = GPIOA->ODR;
 g_hw_debug.gpio_b_idr = GPIOB->IDR; g_hw_debug.gpio_b_odr = GPIOB->ODR;
 g_hw_debug.gpio_c_idr = GPIOC->IDR; g_hw_debug.gpio_c_odr = GPIOC->ODR;
 g_hw_debug.gpio_d_idr = GPIOD->IDR; g_hw_debug.gpio_d_odr = GPIOD->ODR;
 g_hw_debug.gpio_e_idr = GPIOE->IDR; g_hw_debug.gpio_e_odr = GPIOE->ODR;
 g_hw_debug.relay_level = (PIN_RELAY_PORT->ODR & PIN_RELAY) != 0;
 g_hw_debug.select_level = (PIN_SELECT_PORT->ODR & PIN_SELECT) != 0;
 g_hw_debug.tc_cs_level = (PIN_TC_CS_PORT->ODR & PIN_TC_CS) != 0;
 g_hw_debug.encoder_a = (PIN_ENC_A_PORT->IDR & PIN_ENC_A) != 0;
 g_hw_debug.encoder_b = (PIN_ENC_B_PORT->IDR & PIN_ENC_B) != 0;
 g_hw_debug.encoder_button_pressed = board_encoder_button_pressed();
 g_hw_debug.oled_scl = (PIN_OLED_SCL_PORT->IDR & PIN_OLED_SCL) != 0;
 g_hw_debug.oled_sda = (PIN_OLED_SDA_PORT->IDR & PIN_OLED_SDA) != 0;
 g_hw_debug.encoder_counter = board_encoder_counter();
 float voltage;
 g_hw_debug.vf_v = vf_adc_read_voltage(&voltage) ? voltage : NAN;
 g_hw_debug.vdda_v = vf_adc_vdda();
 g_hw_debug.oled_ready = oled_ready(); g_hw_debug.storage_ready = sdlog_ready();
 g_hw_debug.sequence++;
}
