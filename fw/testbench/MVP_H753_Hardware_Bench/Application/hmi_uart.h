#ifndef BENCH_HMI_UART_H
#define BENCH_HMI_UART_H
#include "rs485_port.h"
#include "stm32h7xx_hal.h"
extern volatile rs485_debug_t g_hmi_uart_debug;
extern UART_HandleTypeDef g_hmi_uart;
extern DMA_HandleTypeDef g_hmi_rx_dma;
void hmi_uart_init(uint32_t now);
void hmi_uart_step(uint32_t now);
int hmi_uart_send(const uint8_t *data,size_t length);
int hmi_uart_busy(void);
size_t hmi_uart_take_frame(uint8_t *data,size_t capacity);
/* Dispatched from the shared HAL callbacks; no Modbus processing in ISR. */
void hmi_uart_rx_event(UART_HandleTypeDef *uart,uint16_t size);
void hmi_uart_tx_complete(UART_HandleTypeDef *uart);
void hmi_uart_error(UART_HandleTypeDef *uart);
#endif
