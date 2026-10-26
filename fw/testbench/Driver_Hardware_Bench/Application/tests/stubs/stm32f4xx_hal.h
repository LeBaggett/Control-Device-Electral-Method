/* HOST TEST SHIM ONLY. Never include this directory in the target build. */
#ifndef MOCK_HAL_H
#define MOCK_HAL_H
#include <stdint.h>
typedef struct { unsigned id; } GPIO_TypeDef;
extern GPIO_TypeDef gpio_a, gpio_e;
#define GPIOA (&gpio_a)
#define GPIOE (&gpio_e)
#define GPIO_PIN_0 1U
#define GPIO_PIN_1 2U
#define GPIO_PIN_4 16U
#define GPIO_PIN_5 32U
#define GPIO_PIN_6 64U
#define GPIO_PIN_7 128U
#define GPIO_PIN_2 4U
#define ADC_CHANNEL_10 10U
typedef enum { GPIO_PIN_RESET, GPIO_PIN_SET } GPIO_PinState;
typedef struct { uint32_t Pin, Mode, Pull, Speed; } GPIO_InitTypeDef;
#define GPIO_MODE_OUTPUT_PP 1U
#define GPIO_NOPULL 0U
#define GPIO_PULLDOWN 2U
#define GPIO_SPEED_FREQ_LOW 0U
void mock_clock(unsigned id);
#define __HAL_RCC_GPIOD_CLK_ENABLE() mock_clock(3)
#define __HAL_RCC_GPIOE_CLK_ENABLE() mock_clock(4)
#define __HAL_RCC_GPIOA_CLK_ENABLE() mock_clock(0)
void HAL_GPIO_WritePin(GPIO_TypeDef*, uint16_t, GPIO_PinState);
void HAL_GPIO_Init(GPIO_TypeDef*, GPIO_InitTypeDef*);
void HAL_Init(void);
uint32_t HAL_GetTick(void);
uint32_t HAL_RCC_GetPCLK2Freq(void);
typedef struct {
    uint32_t Mode, Direction, DataSize, CLKPolarity, CLKPhase, NSS,
             FirstBit, TIMode, CRCCalculation, BaudRatePrescaler;
} SPI_InitTypeDef;
typedef struct { void *Instance; SPI_InitTypeDef Init; } SPI_HandleTypeDef;
#define SPI1 ((void*)1)
#define SPI_MODE_MASTER 1U
#define SPI_DIRECTION_2LINES 2U
#define SPI_DATASIZE_8BIT 8U
#define SPI_POLARITY_LOW 0U
#define SPI_PHASE_1EDGE 0U
#define SPI_NSS_SOFT 1U
#define SPI_FIRSTBIT_MSB 0U
#define SPI_TIMODE_DISABLE 0U
#define SPI_CRCCALCULATION_DISABLE 0U
typedef enum { HAL_OK, HAL_ERROR, HAL_BUSY, HAL_TIMEOUT } HAL_StatusTypeDef;
HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef*, uint8_t*, uint8_t*, uint16_t, uint32_t);
typedef struct { volatile uint32_t CTRL, CYCCNT; } DWT_Type;
typedef struct { volatile uint32_t DEMCR; } CoreDebug_Type;
extern DWT_Type mock_dwt;
extern CoreDebug_Type mock_debug;
#define DWT (&mock_dwt)
#define CoreDebug (&mock_debug)
#define CoreDebug_DEMCR_TRCENA_Msk (1UL << 24)
#define DWT_CTRL_CYCCNTENA_Msk 1U
extern uint32_t SystemCoreClock;
void SystemCoreClockUpdate(void);
void mock_nop(void);
#define __NOP() mock_nop()
#define __DSB() ((void)0)
#define __ISB() ((void)0)
typedef struct { void *Instance; } I2C_HandleTypeDef;
typedef struct { uint32_t Period; } TIM_Base_InitTypeDef;
typedef struct { void *Instance; TIM_Base_InitTypeDef Init; uint32_t counter; } TIM_HandleTypeDef;
#define TIM2 ((void*)2)
#define TIM_CHANNEL_ALL 0xFFFFU
#define __HAL_TIM_GET_COUNTER(timer) ((timer)->counter)
typedef struct {
    uint32_t Resolution, DataAlign, ScanConvMode, NbrOfConversion,
             ContinuousConvMode, ExternalTrigConvEdge, EOCSelection, ClockPrescaler;
} ADC_InitTypeDef;
typedef struct { void *Instance; ADC_InitTypeDef Init; } ADC_HandleTypeDef;
typedef struct { uint32_t Channel, Rank, SamplingTime; } ADC_ChannelConfTypeDef;
typedef struct { uint32_t CCR; } ADC_Common_TypeDef;
extern ADC_Common_TypeDef mock_adc_common;
#define ADC (&mock_adc_common)
#define ADC1 ((void*)3)
#define ADC_CCR_TSVREFE (1U << 23)
#define ADC_CHANNEL_VREFINT 17U
#define ADC_RESOLUTION_12B 12U
#define ADC_DATAALIGN_RIGHT 0U
#define ADC_EXTERNALTRIGCONVEDGE_NONE 0U
#define ADC_EOC_SINGLE_CONV 1U
#define ADC_CLOCK_SYNC_PCLK_DIV4 4U
#define ADC_SAMPLETIME_480CYCLES 480U
#define ADC_SAMPLETIME_15CYCLES 15U
#define ENABLE 1U
#define DISABLE 0U
void HAL_Delay(uint32_t);
HAL_StatusTypeDef HAL_I2C_IsDeviceReady(I2C_HandleTypeDef*,uint16_t,uint32_t,uint32_t);
HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef*,uint16_t,uint8_t*,uint16_t,uint32_t);
HAL_StatusTypeDef HAL_TIM_Encoder_Start(TIM_HandleTypeDef*,uint32_t);
GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef*,uint16_t);
HAL_StatusTypeDef HAL_ADC_ConfigChannel(ADC_HandleTypeDef*,ADC_ChannelConfTypeDef*);
HAL_StatusTypeDef HAL_ADC_Start(ADC_HandleTypeDef*);
HAL_StatusTypeDef HAL_ADC_Stop(ADC_HandleTypeDef*);
HAL_StatusTypeDef HAL_ADC_PollForConversion(ADC_HandleTypeDef*,uint32_t);
uint32_t HAL_ADC_GetValue(ADC_HandleTypeDef*);
typedef struct {
    uint32_t ClockEdge,ClockBypass,ClockPowerSave,BusWide,HardwareFlowControl,ClockDiv;
} SD_InitTypeDef;
typedef struct { void *Instance; SD_InitTypeDef Init; } SD_HandleTypeDef;
#define SDIO ((void*)4)
#define SDIO_CLOCK_EDGE_RISING 0U
#define SDIO_CLOCK_BYPASS_DISABLE 0U
#define SDIO_CLOCK_POWER_SAVE_DISABLE 0U
#define SDIO_BUS_WIDE_1B 1U
#define SDIO_BUS_WIDE_4B 4U
#define SDIO_HARDWARE_FLOW_CONTROL_DISABLE 0U
HAL_StatusTypeDef HAL_SD_Init(SD_HandleTypeDef*);
HAL_StatusTypeDef HAL_SD_ConfigWideBusOperation(SD_HandleTypeDef*,uint32_t);
#define HAL_SD_CARD_TRANSFER 4U
#define HAL_SD_CARD_ERROR 255U
uint32_t HAL_SD_GetCardState(SD_HandleTypeDef*);
extern GPIO_TypeDef gpio_b,gpio_c,gpio_d,gpio_h;
#define GPIOB (&gpio_b)
#define GPIOC (&gpio_c)
#define GPIOD (&gpio_d)
#define GPIOH (&gpio_h)
#define GPIO_PIN_3 8U
#define GPIO_PIN_8 256U
#define GPIO_PIN_9 512U
#define GPIO_PIN_10 1024U
#define GPIO_PIN_11 2048U
#define GPIO_PIN_12 4096U
#define GPIO_PIN_13 8192U
#define GPIO_PIN_14 16384U
#define GPIO_PIN_15 32768U
#endif
