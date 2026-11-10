#include "rs485_port.h"
#include "bench_config.h"
#include "pinout.h"
#include <string.h>
/* This buffer is in AXI SRAM (.bss), accessible by DMA1. D-cache stays OFF.
 * Do not move it to DTCM or enable D-cache without a coherency design. */
static uint8_t dma_rx[128] __attribute__((aligned(32)));
static uint8_t tx[8],frame[64],ready[64];
static size_t frame_len,ready_len;
static uint32_t consumed,tx_started,retry_at;
static uint8_t overflow,initialized;
static volatile uint32_t produced,last_cycles,last_ms,rx_error;
static volatile uint16_t last_pos;
static volatile uint8_t transmitting;
UART_HandleTypeDef g_rs485_uart;
DMA_HandleTypeDef g_rs485_rx_dma;
volatile rs485_debug_t g_rs485_debug;
static uint32_t gap_cycles;
static void direction(unsigned tx_active){
 HAL_GPIO_WritePin(BENCH_RS485_DIR_PORT,BENCH_RS485_DIR_PIN,tx_active?GPIO_PIN_SET:GPIO_PIN_RESET);
 g_rs485_debug.direction_tx=tx_active;
}
static int quiet(void){
 return (uint32_t)(HAL_GetTick()-last_ms)>=10U || (uint32_t)(DWT->CYCCNT-last_cycles)>=gap_cycles;
}
static int arm_rx(void){
 produced=consumed=0;last_pos=0;frame_len=ready_len=0;overflow=0;
 HAL_StatusTypeDef result=HAL_UARTEx_ReceiveToIdle_DMA(&g_rs485_uart,dma_rx,sizeof(dma_rx));
 g_rs485_debug.init_status=result;return result==HAL_OK;
}
void rs485_init(uint32_t now){
 GPIO_InitTypeDef gpio={0};RCC_PeriphCLKInitTypeDef clock={0};
 __HAL_RCC_GPIOD_CLK_ENABLE();direction(0);
 gpio.Pin=BENCH_RS485_DIR_PIN;gpio.Mode=GPIO_MODE_OUTPUT_PP;gpio.Pull=GPIO_PULLDOWN;
 gpio.Speed=GPIO_SPEED_FREQ_LOW;HAL_GPIO_Init(BENCH_RS485_DIR_PORT,&gpio);
 gpio.Pin=BENCH_RS485_TX_PIN|BENCH_RS485_RX_PIN;gpio.Mode=GPIO_MODE_AF_PP;
 gpio.Alternate=GPIO_AF7_USART2;gpio.Pull=GPIO_PULLUP;gpio.Speed=GPIO_SPEED_FREQ_HIGH;
 HAL_GPIO_Init(GPIOD,&gpio);
 clock.PeriphClockSelection=RCC_PERIPHCLK_USART2;clock.Usart234578ClockSelection=RCC_USART234578CLKSOURCE_D2PCLK1;
 if(HAL_RCCEx_PeriphCLKConfig(&clock)!=HAL_OK){g_rs485_debug.init_status=10;return;}
 __HAL_RCC_USART2_CLK_ENABLE();__HAL_RCC_DMA1_CLK_ENABLE();
 g_rs485_uart.Instance=USART2;g_rs485_uart.Init.BaudRate=BENCH_RS485_BAUD;
 g_rs485_uart.Init.WordLength=BENCH_RS485_PARITY==0?UART_WORDLENGTH_8B:UART_WORDLENGTH_9B;
 g_rs485_uart.Init.StopBits=UART_STOPBITS_1;
 g_rs485_uart.Init.Parity=BENCH_RS485_PARITY==1?UART_PARITY_EVEN:(BENCH_RS485_PARITY==2?UART_PARITY_ODD:UART_PARITY_NONE);
 g_rs485_uart.Init.Mode=UART_MODE_TX_RX;g_rs485_uart.Init.HwFlowCtl=UART_HWCONTROL_NONE;
 g_rs485_uart.Init.OverSampling=UART_OVERSAMPLING_16;g_rs485_uart.Init.ClockPrescaler=UART_PRESCALER_DIV1;
 if(HAL_UART_Init(&g_rs485_uart)!=HAL_OK){g_rs485_debug.init_status=11;return;}
 g_rs485_rx_dma.Instance=DMA1_Stream0;g_rs485_rx_dma.Init.Request=DMA_REQUEST_USART2_RX;
 g_rs485_rx_dma.Init.Direction=DMA_PERIPH_TO_MEMORY;g_rs485_rx_dma.Init.PeriphInc=DMA_PINC_DISABLE;
 g_rs485_rx_dma.Init.MemInc=DMA_MINC_ENABLE;g_rs485_rx_dma.Init.PeriphDataAlignment=DMA_PDATAALIGN_BYTE;
 g_rs485_rx_dma.Init.MemDataAlignment=DMA_MDATAALIGN_BYTE;g_rs485_rx_dma.Init.Mode=DMA_CIRCULAR;
 g_rs485_rx_dma.Init.Priority=DMA_PRIORITY_HIGH;g_rs485_rx_dma.Init.FIFOMode=DMA_FIFOMODE_DISABLE;
 if(HAL_DMA_Init(&g_rs485_rx_dma)!=HAL_OK){g_rs485_debug.init_status=12;return;}
 __HAL_LINKDMA(&g_rs485_uart,hdmarx,g_rs485_rx_dma);
 CoreDebug->DEMCR|=CoreDebug_DEMCR_TRCENA_Msk;DWT->CTRL|=DWT_CTRL_CYCCNTENA_Msk;
 /* 3.5 characters; use at least 11 bits/character conservatively. High baud: 1.75 ms. */
 uint32_t gap_us=BENCH_RS485_BAUD>19200U?1750U:(38500000U+BENCH_RS485_BAUD-1U)/BENCH_RS485_BAUD;
 gap_cycles=(SystemCoreClock/1000000U)*gap_us;last_cycles=DWT->CYCCNT;last_ms=now;
 /* Same preemption priority prevents DMA and UART callbacks racing their cursors. */
 HAL_NVIC_SetPriority(DMA1_Stream0_IRQn,6,0);HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
 HAL_NVIC_SetPriority(USART2_IRQn,6,0);HAL_NVIC_EnableIRQ(USART2_IRQn);
 initialized=1;rx_error=!arm_rx();retry_at=now;
}
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *h,uint16_t size){
 if(h!=&g_rs485_uart)return;
 uint16_t pos=(uint16_t)(size%sizeof(dma_rx));
 uint16_t delta=(uint16_t)((pos+sizeof(dma_rx)-last_pos)%sizeof(dma_rx));
 if(delta){last_pos=pos;__DMB();produced+=delta;g_rs485_debug.rx_bytes+=delta;
  last_cycles=DWT->CYCCNT;last_ms=HAL_GetTick();g_rs485_debug.rx_events++;}
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *h){
 if(h!=&g_rs485_uart)return;
 /* HAL invokes this on UART TC: last stop bit has left the wire, not just DMA. */
 direction(0);last_cycles=DWT->CYCCNT;last_ms=HAL_GetTick();transmitting=0;
}
void HAL_UART_ErrorCallback(UART_HandleTypeDef *h){
 if(h!=&g_rs485_uart)return;
 g_rs485_debug.uart_error=h->ErrorCode;g_rs485_debug.uart_errors++;rx_error=1;
 direction(0);transmitting=0;
}
int rs485_busy(void){return !initialized || rx_error || transmitting || ready_len || frame_len || !quiet();}
int rs485_send(const uint8_t *data,size_t length){
 if(length!=sizeof(tx) || rs485_busy())return 0;
 memcpy(tx,data,length);direction(1);transmitting=1;tx_started=HAL_GetTick();
 if(HAL_UART_Transmit_IT(&g_rs485_uart,tx,(uint16_t)length)!=HAL_OK){direction(0);transmitting=0;rx_error=1;return 0;}
 g_rs485_debug.tx_frames++;return 1;
}
void rs485_step(uint32_t now){
 if(!initialized)return;
 if(transmitting && (uint32_t)(now-tx_started)>100U){g_rs485_debug.tx_timeouts++;rx_error=1;}
 if(rx_error){
  direction(0);transmitting=0;
  if((uint32_t)(now-retry_at)<100U)return;
  retry_at=now;
  /* Mainloop owns HAL recovery; ISR only records errors. */
  HAL_UART_Abort(&g_rs485_uart);
  uint32_t mask=__get_PRIMASK();__disable_irq();
  rx_error=!arm_rx();last_cycles=DWT->CYCCNT;last_ms=now;__set_PRIMASK(mask);
  g_rs485_debug.restarts++;return;
 }
 uint32_t target=produced;__DMB();uint32_t available=target-consumed;
 if(available>sizeof(dma_rx)){
  consumed=target;overflow=1;frame_len=0;g_rs485_debug.overruns++;
 }else{
  /* Bounded: at most 128 bytes per superloop pass. */
  for(uint32_t i=0;i<available;i++){
   uint8_t byte=dma_rx[consumed++%sizeof(dma_rx)];
   if(frame_len<sizeof(frame) && !overflow)frame[frame_len++]=byte;
   else overflow=1;
  }
  if(produced-consumed>sizeof(dma_rx)){overflow=1;frame_len=0;consumed=produced;g_rs485_debug.overruns++;}
 }
 if(quiet() && consumed==produced){
  if(frame_len && !overflow && !ready_len){memcpy(ready,frame,frame_len);ready_len=frame_len;}
  if(overflow)g_rs485_debug.overruns++;
  frame_len=0;overflow=0;
 }
}
size_t rs485_take_frame(uint8_t *data,size_t capacity){
 size_t n=ready_len;if(n>capacity)n=0;if(n)memcpy(data,ready,n);ready_len=0;return n;
}
