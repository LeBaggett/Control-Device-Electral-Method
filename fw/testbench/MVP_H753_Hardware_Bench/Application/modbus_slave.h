#ifndef BENCH_MODBUS_SLAVE_H
#define BENCH_MODBUS_SLAVE_H
#include <stdint.h>
#include <stddef.h>
typedef enum { SLAVE_IGNORED=0,SLAVE_OK,SLAVE_EXCEPTION,SLAVE_CRC_ERROR,SLAVE_BAD_FRAME } slave_result_t;
/* Registers are a mainloop snapshot; no handlers can write GPIO or settings. */
size_t mb_slave_read(const uint8_t *request,size_t length,uint8_t address,
 const uint16_t *registers,size_t register_count,uint8_t *reply,size_t capacity,
 slave_result_t *result);
#endif
