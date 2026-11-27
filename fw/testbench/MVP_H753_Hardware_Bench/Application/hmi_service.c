#include "hmi_service.h"
#include "hmi_uart.h"
#include "modbus_slave.h"
#include "bench_config.h"
#include "pinout.h"
#include "bench_diag.h"
#include "n1040.h"
#include <stdio.h>
#include <string.h>
volatile hmi_debug_t g_hmi_debug;
static uint16_t registers[HMI_REGISTER_COUNT];
static uint16_t saturated_age(unsigned seen,uint32_t age){return !seen || age>65535U?65535U:(uint16_t)age;}
void hmi_register_snapshot(uint16_t r[HMI_REGISTER_COUNT],uint32_t now){
 memset(r,0,HMI_REGISTER_COUNT*sizeof(*r));
 r[0]=0xca15;r[1]=1;r[2]=0x0208;r[3]=(uint16_t)(now/1000U);
 r[4]=(uint16_t)(g_bench_debug.link_up?1U:0U);r[4]|=0x0300U; /* Power blocked, read-only bench. */
 r[5]=(uint16_t)g_n1040_debug.count;r[7]=(uint16_t)g_rs485_debug.uart_errors;
 r[8]=(uint16_t)g_hmi_debug.requests;r[9]=(uint16_t)g_bench_debug.boot_stage;
 for(unsigned i=0;i<g_n1040_debug.count && i<N1040_MAX_NODES;i++){
  n1040_node_t n=g_n1040_debug.node[i];unsigned b=10+10*i;
  r[b]=(uint16_t)n.address;r[b+1]=(uint16_t)n.pv_tenths;r[b+2]=(uint16_t)n.valid;
  r[b+3]=(uint16_t)n.online;r[b+4]=(uint16_t)n.status3;r[b+5]=(uint16_t)n.last_error;
  r[b+6]=saturated_age(n.pv_seen,now-n.last_pv_ms);r[b+7]=(uint16_t)n.timeouts;
  r[b+8]=(uint16_t)n.responses;r[b+9]=(uint16_t)n.unit_fahrenheit;
  if(n.valid){r[6]++;r[4]|=(uint16_t)(1U<<(i+1));}
 }
 r[40]=BENCH_N1040_POLL_MS;r[41]=(uint16_t)g_bench_debug.max_gap_ms;
 r[42]=(uint16_t)g_hmi_uart_debug.uart_error;r[43]=(uint16_t)g_hmi_uart_debug.uart_errors;
 r[44]=(uint16_t)g_bench_debug.link_changes;r[45]=(uint16_t)g_bench_debug.fatal_code;
 r[46]=(uint16_t)(g_bench_debug.link_up!=0U);
}
void hmi_init(uint32_t now){
 memset((void *)&g_hmi_debug,0,sizeof(g_hmi_debug));
 if(BENCH_HMI_ENABLED)hmi_uart_init(now);
}
void hmi_step(uint32_t now){
 if(!BENCH_HMI_ENABLED)return;
 hmi_uart_step(now);
 uint8_t request[256],reply[133];size_t n=hmi_uart_take_frame(request,sizeof(request));
 if(!n)return;
 hmi_register_snapshot(registers,now);slave_result_t result;
 size_t length=mb_slave_read(request,n,BENCH_HMI_ADDRESS,registers,HMI_REGISTER_COUNT,reply,sizeof(reply),&result);
 if(result==SLAVE_IGNORED){g_hmi_debug.ignored++;return;}
 if(result==SLAVE_CRC_ERROR){g_hmi_debug.crc_errors++;return;}
 if(result==SLAVE_BAD_FRAME){g_hmi_debug.bad_frames++;return;}
 g_hmi_debug.requests++;g_hmi_debug.last_request_ms=now;
 if(result==SLAVE_EXCEPTION)g_hmi_debug.exceptions++;
 if(length && !hmi_uart_busy() && hmi_uart_send(reply,length))g_hmi_debug.responses++;
 else g_hmi_debug.tx_failures++;
}
int hmi_json(char *out,size_t capacity){
 if(!out || !capacity)return -1;
 int n=snprintf(out,capacity,"{\"enabled\":%u,\"uart\":\"UART4\",\"tx_pin\":\"%s\",\"rx_pin\":\"%s\",\"baud\":%u,\"address\":%u,\"parity\":%u,\"readonly\":true,\"map_version\":1,\"init_status\":%lu,\"requests\":%lu,\"responses\":%lu,\"exceptions\":%lu,\"crc_errors\":%lu,\"bad_frames\":%lu,\"ignored\":%lu,\"tx_failures\":%lu,\"last_request_ms\":%lu,\"rx_bytes\":%lu,\"uart_error\":%lu,\"uart_errors\":%lu,\"restarts\":%lu,\"tx_timeouts\":%lu}",
 BENCH_HMI_ENABLED,BENCH_HMI_TX_NAME,BENCH_HMI_RX_NAME,BENCH_HMI_BAUD,BENCH_HMI_ADDRESS,BENCH_HMI_PARITY,
 (unsigned long)g_hmi_uart_debug.init_status,(unsigned long)g_hmi_debug.requests,
 (unsigned long)g_hmi_debug.responses,(unsigned long)g_hmi_debug.exceptions,
 (unsigned long)g_hmi_debug.crc_errors,(unsigned long)g_hmi_debug.bad_frames,
 (unsigned long)g_hmi_debug.ignored,(unsigned long)g_hmi_debug.tx_failures,
 (unsigned long)g_hmi_debug.last_request_ms,(unsigned long)g_hmi_uart_debug.rx_bytes,
 (unsigned long)g_hmi_uart_debug.uart_error,(unsigned long)g_hmi_uart_debug.uart_errors,
 (unsigned long)g_hmi_uart_debug.restarts,(unsigned long)g_hmi_uart_debug.tx_timeouts);
 if(n<0 || (size_t)n>=capacity){out[0]=0;return -1;}return n;
}
