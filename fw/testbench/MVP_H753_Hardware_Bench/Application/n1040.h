#ifndef N1040_H
#define N1040_H
#include <stdint.h>
#include <stddef.h>
#define N1040_MAX_NODES 3
/* error: 0 none, 1 timeout, 2 CRC, 3 address, 4 function, 5 length,
 * 6 Modbus exception, 7 short frame, 8 UART/transport, 9 wrong device ID.
 * Sensor validity is independent: status3 bits 0..6 invalidate temperature. */
typedef struct {
 uint32_t address,valid,online,pv_seen,status_seen,last_pv_ms,last_status_ms;
 int32_t pv_tenths;
 uint32_t status1,status2,status3,device_id,firmware,unit_fahrenheit;
 uint32_t requests,responses,timeouts,crc_errors,protocol_errors,exceptions;
 uint32_t last_error,exception_code,last_rtt_ms;
} n1040_node_t;
typedef struct {uint32_t enabled,count,active_node,waiting,register_address,requests,unsolicited;
 n1040_node_t node[N1040_MAX_NODES];} n1040_debug_t;
extern volatile n1040_debug_t g_n1040_debug;
void n1040_init(uint32_t now);
void n1040_step(uint32_t now);
int n1040_json(char *out,size_t capacity);
#endif
