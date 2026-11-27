from pathlib import Path
import tempfile,subprocess,json
root=Path(__file__).resolve().parents[2]
stub='''#include "rs485_port.h"
extern volatile rs485_debug_t g_hmi_uart_debug;
void hmi_uart_init(uint32_t now);void hmi_uart_step(uint32_t now);
int hmi_uart_send(const uint8_t *data,size_t length);int hmi_uart_busy(void);
size_t hmi_uart_take_frame(uint8_t *data,size_t capacity);
'''
harness=r'''
#include "hmi_service.h"
#include "hmi_uart.h"
#include "bench_config.h"
#include "bench_diag.h"
#include "n1040.h"
#include "modbus_slave.h"
#include "modbus_read.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
volatile bench_diag_t g_bench_debug;
volatile n1040_debug_t g_n1040_debug;
volatile rs485_debug_t g_rs485_debug,g_hmi_uart_debug;
static uint8_t incoming[256],reply[133];static size_t incoming_size,reply_size;static int busy;
void hmi_uart_init(uint32_t now){(void)now;busy=0;incoming_size=reply_size=0;}
void hmi_uart_step(uint32_t now){(void)now;}
int hmi_uart_busy(void){return busy;}
int hmi_uart_send(const uint8_t*d,size_t n){assert(n<=sizeof(reply));memcpy(reply,d,n);reply_size=n;return 1;}
size_t hmi_uart_take_frame(uint8_t*d,size_t n){assert(n>=incoming_size);memcpy(d,incoming,incoming_size);size_t result=incoming_size;incoming_size=0;return result;}
static void crc(uint8_t*d,size_t n){uint16_t c=mb_crc16(d,n);d[n]=(uint8_t)c;d[n+1]=(uint8_t)(c>>8);}
static void ask(uint8_t address,uint16_t first,uint16_t count,uint8_t function){
 assert(mb_read_request(incoming,address,first,count)==8);incoming[1]=function;crc(incoming,6);incoming_size=8;reply_size=0;
}
int main(void){
 g_bench_debug.link_up=1;g_bench_debug.boot_stage=5;g_n1040_debug.count=3;
 for(unsigned i=0;i<3;i++){
  g_n1040_debug.node[i].address=247-i;g_n1040_debug.node[i].valid=i<2;
  g_n1040_debug.node[i].online=i<2;g_n1040_debug.node[i].pv_seen=i<2;
  g_n1040_debug.node[i].pv_tenths=i==1?-55:266;g_n1040_debug.node[i].last_pv_ms=990;
 }
 uint16_t r[64];hmi_register_snapshot(r,1000);
 assert(r[0]==0xca15 && r[1]==1 && r[4]==0x0307 && r[5]==3 && r[6]==2);
 assert(r[10]==247 && r[11]==266 && r[12]==1 && r[16]==10);
 assert(r[20]==246 && mb_signed_word(r[21])==-55);
 assert(r[30]==245 && r[32]==0 && r[36]==65535);
 hmi_register_snapshot(r,100000);assert(r[16]==65535);
 slave_result_t result;uint8_t q[8],out[133];assert(mb_read_request(q,1,0,64)==8);
 assert(mb_slave_read(q,8,1,r,64,out,sizeof(out),&result)==133 && result==SLAVE_OK);
 assert(out[2]==128 && mb_crc16(out,131)==((uint16_t)out[131]|((uint16_t)out[132]<<8)));
 assert(mb_slave_read(q,8,1,r,64,out,132,&result)==0 && result==SLAVE_BAD_FRAME);
 assert(mb_slave_read(q,8,2,r,64,out,133,&result)==0 && result==SLAVE_IGNORED);
 q[0]=0;crc(q,6);assert(!mb_slave_read(q,8,1,r,64,out,133,&result) && result==SLAVE_IGNORED);
 mb_read_request(q,1,63,2);assert(mb_slave_read(q,8,1,r,64,out,133,&result)==5 && out[2]==2);
 mb_read_request(q,1,0,65);assert(mb_slave_read(q,8,1,r,64,out,133,&result)==5 && out[2]==3);
 q[4]=q[5]=0;crc(q,6);assert(mb_slave_read(q,8,1,r,64,out,133,&result)==5 && out[2]==3);
 q[0]=1;q[1]=6;crc(q,6);assert(mb_slave_read(q,8,1,r,64,out,133,&result)==5 && out[1]==0x86 && out[2]==1);
 assert(!mb_slave_read(out,5,1,r,64,reply,133,&result) && result==SLAVE_IGNORED);
 q[1]=3;q[4]=0;q[5]=1;crc(q,6);q[3]^=1;
 assert(!mb_slave_read(q,8,1,r,64,out,133,&result) && result==SLAVE_CRC_ERROR);
 assert(!mb_slave_read(q,3,1,r,64,out,133,&result) && result==SLAVE_BAD_FRAME);
 hmi_init(0);ask(BENCH_HMI_ADDRESS,10,30,3);hmi_step(1000);
 assert(reply_size==65 && reply[1]==3 && reply[5]==1 && reply[6]==10);
 assert(g_hmi_debug.requests==1 && g_hmi_debug.responses==1);
 ask(BENCH_HMI_ADDRESS,0,1,4);hmi_step(1100);assert(reply_size==7 && reply[1]==4 && reply[3]==0xca && reply[4]==0x15);
 ask(BENCH_HMI_ADDRESS,0,1,6);hmi_step(1200);assert(reply_size==5 && g_hmi_debug.exceptions==1);
 assert(g_n1040_debug.node[0].pv_tenths==266 && g_bench_debug.link_up==1);
 ask(BENCH_HMI_ADDRESS,0,1,3);incoming[3]^=1;hmi_step(1300);assert(!reply_size && g_hmi_debug.crc_errors==1);
 ask(2,0,1,3);hmi_step(1400);assert(!reply_size && g_hmi_debug.ignored==1);
 ask(BENCH_HMI_ADDRESS,0,1,3);busy=1;hmi_step(1500);assert(!reply_size && g_hmi_debug.tx_failures==1);
 char data[1024],small[4];assert(hmi_json(data,sizeof(data))>0);
 assert(hmi_json(small,sizeof(small))==-1 && small[0]==0);assert(hmi_json(NULL,0)==-1);puts(data);
 return 0;
}
'''
with tempfile.TemporaryDirectory(prefix='hmi-tests-') as td:
 d=Path(td);(d/'hmi_uart.h').write_text(stub);(d/'test.c').write_text(harness)
 (d/'hmi_service.c').write_text((root/'Application/hmi_service.c').read_text())
 (d/'pinout.h').write_text((root/'Application/board/pinout.h').read_text().replace('#include "stm32h7xx_hal.h"',''))
 exe=d/'test.exe'
 subprocess.run(['gcc','-std=c11','-Wall','-Wextra','-Werror','-I'+str(d),'-I'+str(root/'Application'),
  str(d/'test.c'),str(d/'hmi_service.c'),str(root/'Application/modbus_slave.c'),str(root/'Application/modbus_read.c'),'-o',str(exe)],check=True)
 result=json.loads(subprocess.check_output([str(exe)],text=True))
 assert result['readonly'] is True and result['address']==1 and result['baud']==9600
 assert result['tx_pin']=='PD1' and result['rx_pin']=='PD0'
 assert result['requests']==4 and result['responses']==3 and result['tx_failures']==1
 print('PASS: real HMI map/service and Modbus slave: signed values, validity, age, FC03/04, full map, CRC, bounds, silent broadcast, rejected writes, echoes and bounded JSON')
