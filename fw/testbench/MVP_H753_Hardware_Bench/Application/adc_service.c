#include "adc_service.h"
#include "adc_stats.h"
#include "bench_config.h"
#include "pinout.h"
#include <stdio.h>

ADC_HandleTypeDef g_adc_handle;
DMA_HandleTypeDef g_adc_dma;
volatile adc_debug_t g_adc_debug;
static TIM_HandleTypeDef timer;
/* AXI SRAM (.bss), reachable by DMA1. D-cache remains disabled globally.
 * Each completed half is copied before DMA wraps; no float/HTTP work in ISR. */
static uint16_t dma_samples[64] __attribute__((aligned(32)));
static volatile uint16_t mailbox[32];
static volatile uint32_t sequence,block_ms;
static uint32_t consumed;

static void publish(ADC_HandleTypeDef *adc,unsigned offset){
 if(adc!=&g_adc_handle)return;
 sequence++;__DMB();
 for(unsigned i=0;i<32;i++)mailbox[i]=dma_samples[offset+i];
 block_ms=HAL_GetTick();__DMB();sequence++;
}
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *adc){publish(adc,0);}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *adc){publish(adc,32);}
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *adc){
 if(adc==&g_adc_handle){g_adc_debug.error|=HAL_ADC_GetError(adc);g_adc_debug.valid=0;}
}
static int checked(HAL_StatusTypeDef status){
 g_adc_debug.init_status=(uint32_t)status;
 return status==HAL_OK;
}
void adc_init(void){
 /* CKPER=HSI64, ADC asynchronous /4 = 16 MHz. No PLL or UART clock changes. */
 g_adc_debug.phase=1;
 RCC_PeriphCLKInitTypeDef clock={0};
 clock.PeriphClockSelection=RCC_PERIPHCLK_ADC|RCC_PERIPHCLK_CKPER;
 clock.CkperClockSelection=RCC_CLKPSOURCE_HSI;
 clock.AdcClockSelection=RCC_ADCCLKSOURCE_CLKP;
 if(!checked(HAL_RCCEx_PeriphCLKConfig(&clock)))return;
 BENCH_ADC_GPIO_CLOCK_ENABLE();__HAL_RCC_ADC12_CLK_ENABLE();
 __HAL_RCC_DMA1_CLK_ENABLE();__HAL_RCC_TIM6_CLK_ENABLE();
 GPIO_InitTypeDef gpio={0};gpio.Pin=BENCH_ADC_PIN;gpio.Mode=GPIO_MODE_ANALOG;gpio.Pull=GPIO_NOPULL;
 HAL_GPIO_Init(BENCH_ADC_PORT,&gpio);
 g_adc_dma.Instance=DMA1_Stream2;g_adc_dma.Init.Request=DMA_REQUEST_ADC1;
 g_adc_dma.Init.Direction=DMA_PERIPH_TO_MEMORY;g_adc_dma.Init.PeriphInc=DMA_PINC_DISABLE;
 g_adc_dma.Init.MemInc=DMA_MINC_ENABLE;g_adc_dma.Init.PeriphDataAlignment=DMA_PDATAALIGN_HALFWORD;
 g_adc_dma.Init.MemDataAlignment=DMA_MDATAALIGN_HALFWORD;g_adc_dma.Init.Mode=DMA_CIRCULAR;
 g_adc_dma.Init.Priority=DMA_PRIORITY_HIGH;g_adc_dma.Init.FIFOMode=DMA_FIFOMODE_DISABLE;
 g_adc_debug.phase=2;if(!checked(HAL_DMA_Init(&g_adc_dma)))return;
 g_adc_handle.Instance=ADC1;__HAL_LINKDMA(&g_adc_handle,DMA_Handle,g_adc_dma);
 g_adc_handle.Init.ClockPrescaler=ADC_CLOCK_ASYNC_DIV4;
 g_adc_handle.Init.Resolution=ADC_RESOLUTION_16B;
 g_adc_handle.Init.ScanConvMode=ADC_SCAN_DISABLE;g_adc_handle.Init.EOCSelection=ADC_EOC_SINGLE_CONV;
 g_adc_handle.Init.NbrOfConversion=1;g_adc_handle.Init.ExternalTrigConv=ADC_EXTERNALTRIG_T6_TRGO;
 g_adc_handle.Init.ExternalTrigConvEdge=ADC_EXTERNALTRIGCONVEDGE_RISING;
 g_adc_handle.Init.ConversionDataManagement=ADC_CONVERSIONDATA_DMA_CIRCULAR;
 g_adc_handle.Init.Overrun=ADC_OVR_DATA_PRESERVED;
 g_adc_debug.phase=3;if(!checked(HAL_ADC_Init(&g_adc_handle)))return;
 ADC_ChannelConfTypeDef channel={0};channel.Channel=BENCH_ADC_CHANNEL;channel.Rank=ADC_REGULAR_RANK_1;
 channel.SamplingTime=ADC_SAMPLETIME_387CYCLES_5;channel.SingleDiff=ADC_SINGLE_ENDED;
 channel.OffsetNumber=ADC_OFFSET_NONE;
 if(!checked(HAL_ADC_ConfigChannel(&g_adc_handle,&channel)))return;
 g_adc_debug.phase=4;
 if(!checked(HAL_ADCEx_Calibration_Start(&g_adc_handle,ADC_CALIB_OFFSET_LINEARITY,ADC_SINGLE_ENDED)))return;
 /* Existing board clock: APB1 /2 and TIMPRE=0 -> timer clock=2*PCLK1.
 * Refuse unsupported clock configurations rather than silently change cadence. */
 uint32_t hz=HAL_RCC_GetPCLK1Freq()*2U;
 g_adc_debug.phase=5;
 if((RCC->CFGR&RCC_CFGR_TIMPRE)!=0 || HAL_RCC_GetHCLKFreq()!=hz || hz%1000000U){
  g_adc_debug.init_status=HAL_ERROR;return;
 }
 timer.Instance=TIM6;timer.Init.Prescaler=hz/1000000U-1U;
 timer.Init.Period=1000000U/BENCH_ADC_SAMPLE_HZ-1U;timer.Init.CounterMode=TIM_COUNTERMODE_UP;
 if(!checked(HAL_TIM_Base_Init(&timer)))return;
 TIM_MasterConfigTypeDef trigger={0};trigger.MasterOutputTrigger=TIM_TRGO_UPDATE;
 trigger.MasterSlaveMode=TIM_MASTERSLAVEMODE_DISABLE;
 if(!checked(HAL_TIMEx_MasterConfigSynchronization(&timer,&trigger)))return;
 HAL_NVIC_SetPriority(DMA1_Stream2_IRQn,7,0);HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
 HAL_NVIC_SetPriority(ADC_IRQn,7,0);HAL_NVIC_EnableIRQ(ADC_IRQn);
 g_adc_debug.phase=6;
 /* DMA is armed first. Timer starts last, after calibration and complete setup. */
 if(!checked(HAL_ADC_Start_DMA(&g_adc_handle,(uint32_t *)dma_samples,64)))return;
 if(!checked(HAL_TIM_Base_Start(&timer))){(void)HAL_ADC_Stop_DMA(&g_adc_handle);return;}
 g_adc_debug.phase=7;
}
void adc_step(uint32_t now){
 if(g_adc_debug.error){
  if(g_adc_debug.phase==7){(void)HAL_TIM_Base_Stop(&timer);(void)HAL_ADC_Stop_DMA(&g_adc_handle);g_adc_debug.phase=8;}
  g_adc_debug.valid=0;return;
 }
 uint32_t first=sequence;
 if(first!=consumed && !(first&1U)){
  uint16_t values[32];__DMB();
  for(unsigned i=0;i<32;i++)values[i]=mailbox[i];
  uint32_t timestamp=block_ms;__DMB();
  if(first==sequence){
   adc_stats_t stats=adc_block_stats(values,BENCH_ADC_REFERENCE_MV);
   uint32_t delta=(first-consumed)/2U;
   g_adc_debug.skipped_blocks+=delta-1U;consumed=first;
   g_adc_debug.blocks+=delta;g_adc_debug.last_block_ms=timestamp;
   g_adc_debug.mean_raw=stats.mean;g_adc_debug.min_raw=stats.minimum;g_adc_debug.max_raw=stats.maximum;
   g_adc_debug.last_raw=values[31];
   /* Nominal ADC input voltage, not calibrated DUT Vf. Use unrounded block mean. */
   g_adc_debug.millivolts=stats.millivolts;
   g_adc_debug.input_voltage=stats.volts;
  }
 }
 g_adc_debug.valid=g_adc_debug.phase==7 && g_adc_debug.blocks && (uint32_t)(now-g_adc_debug.last_block_ms)<=BENCH_ADC_STALE_MS;
}
int adc_json(char *out,size_t capacity){
 if(!out || !capacity)return -1;
 int n=snprintf(out,capacity,"{\"pin\":\"%s\",\"bits\":16,\"sample_hz\":%u,\"reference_mv_nominal\":%u,\"phase\":%lu,\"init_status\":%lu,\"error\":%lu,\"valid\":%lu,\"mean_raw\":%lu,\"min_raw\":%lu,\"max_raw\":%lu,\"input_mv\":%lu,\"blocks\":%lu,\"skipped_blocks\":%lu}",BENCH_ADC_PIN_NAME,BENCH_ADC_SAMPLE_HZ,BENCH_ADC_REFERENCE_MV,
 (unsigned long)g_adc_debug.phase,(unsigned long)g_adc_debug.init_status,(unsigned long)g_adc_debug.error,
 (unsigned long)g_adc_debug.valid,(unsigned long)g_adc_debug.mean_raw,(unsigned long)g_adc_debug.min_raw,
 (unsigned long)g_adc_debug.max_raw,(unsigned long)g_adc_debug.millivolts,(unsigned long)g_adc_debug.blocks,
 (unsigned long)g_adc_debug.skipped_blocks);
 if(n<0 || (size_t)n>=capacity){out[0]=0;return -1;}
 return n;
}
