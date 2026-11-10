#include "n1040.h"
#include "modbus_read.h"
#include "rs485_port.h"
#include "bench_config.h"
#include <stdio.h>
#include <string.h>
volatile n1040_debug_t g_n1040_debug;
static const uint8_t addresses[N1040_MAX_NODES]={BENCH_N1040_ADDR1,BENCH_N1040_ADDR2,BENCH_N1040_ADDR3};
static uint32_t scheduled,sent,observed_uart_errors;
static unsigned slot,kind;
static uint16_t expected_count;
static void fail(volatile n1040_node_t *n,unsigned error){
 n->last_error=error;n->valid=0;n->online=0;n->pv_seen=0;n->status_seen=0;
}
void n1040_init(uint32_t now){
 memset((void *)&g_n1040_debug,0,sizeof(g_n1040_debug));
 g_n1040_debug.enabled=BENCH_N1040_ENABLED;g_n1040_debug.count=BENCH_N1040_COUNT;
 for(unsigned i=0;i<BENCH_N1040_COUNT;i++)g_n1040_debug.node[i].address=addresses[i];
 slot=kind=0;scheduled=now;observed_uart_errors=0;
 if(BENCH_N1040_ENABLED)rs485_init(now);
}
static void advance(void){
 g_n1040_debug.waiting=0;
 if(kind==0)kind=1;else{kind=0;slot=(slot+1)%BENCH_N1040_COUNT;}
}
void n1040_step(uint32_t now){
 if(!BENCH_N1040_ENABLED)return;
 rs485_step(now);
 volatile n1040_node_t *n=&g_n1040_debug.node[slot];
 if(g_n1040_debug.waiting && observed_uart_errors!=g_rs485_debug.uart_errors){
  fail(n,8);advance();
 }
 observed_uart_errors=g_rs485_debug.uart_errors;
 uint8_t frame[64];size_t length=rs485_take_frame(frame,sizeof(frame));
 if(length){
  if(!g_n1040_debug.waiting)g_n1040_debug.unsolicited++;
  else{
   uint16_t words[5];uint8_t exception;
   mb_result_t result=mb_read_response(frame,length,(uint8_t)n->address,expected_count,words,&exception);
   n->exception_code=exception;
   if(result==MB_OK){
    n->responses++;n->last_error=0;n->last_rtt_ms=now-sent;n->online=1;
    if(kind==0){
     /* A unit/sensor-state change must not relabel an older PV as a valid
      * sample in the new state. Wait for the next successful PV request. */
     if(((n->status3^words[4])&0x7fU) || n->unit_fahrenheit!=((words[3]>>9)&1U))n->pv_seen=0;
     n->status1=words[0];n->firmware=words[1];n->device_id=words[2];
     n->status2=words[3];n->status3=words[4];n->unit_fahrenheit=(words[3]>>9)&1U;
     n->last_status_ms=now;n->status_seen=1;
     if(n->device_id!=68U)fail(n,9);
    }else{n->pv_tenths=mb_signed_word(words[0]);n->last_pv_ms=now;n->pv_seen=1;}
   }else{
    unsigned error=result==MB_SHORT?7U:(unsigned)result;
    fail(n,error);
    if(result==MB_CRC)n->crc_errors++;
    else if(result==MB_EXCEPTION)n->exceptions++;
    else n->protocol_errors++;
   }
   advance();
  }
 }
 if(g_n1040_debug.waiting && (uint32_t)(now-sent)>=BENCH_N1040_TIMEOUT_MS){
  n->timeouts++;fail(n,1);advance();
 }
 /* Every response needs a matching CRC/address/function/length. Temperature
  * additionally needs fresh PV + status, expected device ID and no sensor flags.
  * Expose old readings for diagnosis, but never label them current/valid. */
 for(unsigned i=0;i<BENCH_N1040_COUNT;i++){
  volatile n1040_node_t *v=&g_n1040_debug.node[i];
  unsigned fresh=v->pv_seen && v->status_seen && now-v->last_pv_ms<=BENCH_N1040_STALE_MS && now-v->last_status_ms<=BENCH_N1040_STALE_MS;
  v->valid=fresh && !v->last_error && v->device_id==68U && !(v->status3&0x7fU);
  if(!fresh)v->online=0;
 }
 if(!g_n1040_debug.waiting && (uint32_t)(now-scheduled)>=BENCH_N1040_POLL_MS && !rs485_busy()){
  uint8_t request[8];uint16_t reg=kind==0?6:1;expected_count=kind==0?5:1;
  if(mb_read_request(request,addresses[slot],reg,expected_count) && rs485_send(request,sizeof(request))){
   n=&g_n1040_debug.node[slot];n->requests++;g_n1040_debug.requests++;
   g_n1040_debug.active_node=slot;g_n1040_debug.register_address=reg;
   g_n1040_debug.waiting=1;sent=scheduled=now;
  }
 }
}
int n1040_json(char *out,size_t capacity){
 if(!out || !capacity)return -1;
 int n=snprintf(out,capacity,"{\"enabled\":%lu,\"baud\":%u,\"parity\":%u,\"count\":%lu,\"waiting\":%lu,\"uart_init\":%lu,\"uart_error\":%lu,\"uart_errors\":%lu,\"rx_bytes\":%lu,\"overruns\":%lu,\"restarts\":%lu,\"nodes\":[",
 (unsigned long)g_n1040_debug.enabled,BENCH_RS485_BAUD,BENCH_RS485_PARITY,
 (unsigned long)g_n1040_debug.count,(unsigned long)g_n1040_debug.waiting,
 (unsigned long)g_rs485_debug.init_status,(unsigned long)g_rs485_debug.uart_error,
 (unsigned long)g_rs485_debug.uart_errors,(unsigned long)g_rs485_debug.rx_bytes,
 (unsigned long)g_rs485_debug.overruns,(unsigned long)g_rs485_debug.restarts);
 if(n<0 || (size_t)n>=capacity)goto truncated;
 size_t used=(size_t)n;
 for(unsigned i=0;i<BENCH_N1040_COUNT;i++){
  n1040_node_t v=g_n1040_debug.node[i];
  n=snprintf(out+used,capacity-used,"%s{\"address\":%lu,\"valid\":%lu,\"online\":%lu,\"pv_tenths\":%ld,\"unit\":\"%s\",\"status1\":%lu,\"status2\":%lu,\"status3\":%lu,\"id\":%lu,\"firmware\":%lu,\"pv_seen\":%lu,\"status_seen\":%lu,\"last_pv_ms\":%lu,\"last_status_ms\":%lu,\"requests\":%lu,\"responses\":%lu,\"timeouts\":%lu,\"crc_errors\":%lu,\"protocol_errors\":%lu,\"exceptions\":%lu,\"error\":%lu,\"exception\":%lu,\"rtt_ms\":%lu}",
  i?",":"",(unsigned long)v.address,(unsigned long)v.valid,(unsigned long)v.online,
  (long)v.pv_tenths,v.unit_fahrenheit?"F":"C",(unsigned long)v.status1,(unsigned long)v.status2,
  (unsigned long)v.status3,(unsigned long)v.device_id,(unsigned long)v.firmware,
  (unsigned long)v.pv_seen,(unsigned long)v.status_seen,(unsigned long)v.last_pv_ms,(unsigned long)v.last_status_ms,
  (unsigned long)v.requests,(unsigned long)v.responses,(unsigned long)v.timeouts,(unsigned long)v.crc_errors,
  (unsigned long)v.protocol_errors,(unsigned long)v.exceptions,(unsigned long)v.last_error,
  (unsigned long)v.exception_code,(unsigned long)v.last_rtt_ms);
  if(n<0 || (size_t)n>=capacity-used)goto truncated;
  used+=(size_t)n;
 }
 if(capacity-used<3)goto truncated;
 out[used++]=']';out[used++]='}';out[used]=0;return (int)used;
truncated:out[0]=0;return -1;
}
