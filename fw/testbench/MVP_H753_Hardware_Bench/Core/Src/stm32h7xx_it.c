#include "main.h"
#include "bench_diag.h"
extern ETH_HandleTypeDef EthHandle;
extern UART_HandleTypeDef g_rs485_uart;
extern DMA_HandleTypeDef g_rs485_rx_dma;
void USART2_IRQHandler(void){HAL_UART_IRQHandler(&g_rs485_uart);}
void DMA1_Stream0_IRQHandler(void){HAL_DMA_IRQHandler(&g_rs485_rx_dma);}
void SysTick_Handler(void){HAL_IncTick();}
void ETH_IRQHandler(void){HAL_ETH_IRQHandler(&EthHandle);}
void NMI_Handler(void){bench_fatal(101);}
void HardFault_Handler(void){bench_fatal(102);}
void MemManage_Handler(void){bench_fatal(103);}
void BusFault_Handler(void){bench_fatal(104);}
void UsageFault_Handler(void){bench_fatal(105);}
/* ISR callbacks only retain counters. Releasing pbufs stays in the mainloop. */
void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *h){(void)h;g_bench_debug.rx_irq++;}
void HAL_ETH_TxCpltCallback(ETH_HandleTypeDef *h){(void)h;g_bench_debug.tx_irq++;}
void HAL_ETH_ErrorCallback(ETH_HandleTypeDef *h){g_bench_debug.eth_irq_errors++;g_bench_debug.eth_error=h->ErrorCode;g_bench_debug.dma_error=h->DMAErrorCode;}
