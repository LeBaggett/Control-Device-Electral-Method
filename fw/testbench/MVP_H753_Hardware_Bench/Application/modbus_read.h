#ifndef MODBUS_READ_H
#define MODBUS_READ_H
#include <stddef.h>
#include <stdint.h>
typedef enum { MB_OK=0, MB_SHORT=1, MB_CRC=2, MB_ADDRESS=3,
 MB_FUNCTION=4, MB_LENGTH=5, MB_EXCEPTION=6 } mb_result_t;
uint16_t mb_crc16(const uint8_t *data,size_t len);
/* Only function 03 is exposed: this bench cannot write instrument settings. */
size_t mb_read_request(uint8_t out[8],uint8_t address,uint16_t reg,uint16_t count);
mb_result_t mb_read_response(const uint8_t *data,size_t len,uint8_t address,
 uint16_t count,uint16_t *words,uint8_t *exception);
int32_t mb_signed_word(uint16_t word);
#endif
