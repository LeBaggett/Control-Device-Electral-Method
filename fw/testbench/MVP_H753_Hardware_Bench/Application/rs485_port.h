#ifndef RS485_PORT_H
#define RS485_PORT_H
#include <stdint.h>
#include <stddef.h>
typedef struct {uint32_t init_status,rx_events,rx_bytes,tx_frames,uart_error,
 uart_errors,overruns,restarts,tx_timeouts,direction_tx;} rs485_debug_t;
extern volatile rs485_debug_t g_rs485_debug;
void rs485_init(uint32_t now);
void rs485_step(uint32_t now);
int rs485_send(const uint8_t *data,size_t length);
int rs485_busy(void);
size_t rs485_take_frame(uint8_t *data,size_t capacity);
#endif
