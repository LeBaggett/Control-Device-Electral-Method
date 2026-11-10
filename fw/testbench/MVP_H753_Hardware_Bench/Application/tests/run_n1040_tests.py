"""Host tests of the actual read-only codec and scheduler with a fake wire.
No hardware validation is implied; UART/DMA/turnaround still require bench tests.
"""
from pathlib import Path
import subprocess,tempfile,json,re
root=Path(__file__).resolve().parents[2]
harness=r'''
#include "n1040.h"
#include "rs485_port.h"
#include "modbus_read.h"
#include "bench_config.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
volatile rs485_debug_t g_rs485_debug;
static uint8_t incoming[64],request[8];static size_t incoming_size;static int blocked;
void rs485_init(uint32_t now){(void)now;memset((void*)&g_rs485_debug,0,sizeof(g_rs485_debug));incoming_size=0;blocked=0;}
void rs485_step(uint32_t now){(void)now;}
int rs485_busy(void){return blocked;}
int rs485_send(const uint8_t*d,size_t n){assert(n==8);assert(d[1]==3);memcpy(request,d,n);return 1;}
size_t rs485_take_frame(uint8_t*d,size_t n){assert(n>=incoming_size);size_t len=incoming_size;memcpy(d,incoming,len);incoming_size=0;return len;}
static void crc(uint8_t*b,size_t n){uint16_t c=mb_crc16(b,n);b[n]=(uint8_t)c;b[n+1]=(uint8_t)(c>>8);}
static void respond(const uint16_t*w,unsigned count,uint32_t now){
 incoming[0]=request[0];incoming[1]=3;incoming[2]=(uint8_t)(count*2);
 for(unsigned i=0;i<count;i++){incoming[3+2*i]=(uint8_t)(w[i]>>8);incoming[4+2*i]=(uint8_t)w[i];}
 crc(incoming,3+count*2);incoming_size=5+count*2;n1040_step(now);
}
int main(void){
 uint8_t b[8],exc;uint16_t words[125];
 assert(mb_crc16((const uint8_t*)"123456789",9)==0x4b37);
 assert(mb_read_request(b,1,0,10)==8 && b[6]==0xc5 && b[7]==0xcd);
 assert(mb_read_request(b,0,1,1)==0 && mb_read_request(b,248,1,1)==0);
 assert(mb_read_request(b,1,1,0)==0 && mb_read_request(b,1,1,126)==0);
 assert(mb_signed_word(0xff9c)==-100 && mb_signed_word(0x8000)==-32768 && mb_signed_word(0x7fff)==32767);
 uint8_t f[]={1,3,2,0xff,0x9c,0,0};crc(f,5);
 assert(mb_read_response(f,7,1,1,words,&exc)==MB_OK && mb_signed_word(words[0])==-100);
 assert(mb_read_response(f,7,2,1,words,&exc)==MB_ADDRESS);
 assert(mb_read_response(f,7,1,2,words,&exc)==MB_LENGTH);
 assert(mb_read_response(f,7,1,126,words,&exc)==MB_LENGTH);
 f[3]^=1;assert(mb_read_response(f,7,1,1,words,&exc)==MB_CRC);f[3]^=1;
 assert(mb_read_response(f,4,1,1,words,&exc)==MB_SHORT);
 uint8_t e[]={1,0x83,2,0,0};crc(e,3);
 assert(mb_read_response(e,5,1,1,words,&exc)==MB_EXCEPTION && exc==2);
 e[1]=4;crc(e,3);assert(mb_read_response(e,5,1,1,words,&exc)==MB_FUNCTION);
 const uint16_t status[]={0,200,68,0,0};const uint16_t pv[]={298};
 n1040_init(0);assert(!g_n1040_debug.node[0].valid);
 /* Sequential round-robin, status then PV for every configured instrument. */
 for(unsigned i=0;i<BENCH_N1040_COUNT;i++){
  uint32_t t=250+500*i;n1040_step(t);assert(request[0]==247-i && request[3]==6 && request[5]==5);
  respond(status,5,t+10);assert(!g_n1040_debug.node[i].valid);
  n1040_step(t+250);assert(request[3]==1 && request[5]==1);respond(pv,1,t+260);
  assert(g_n1040_debug.node[i].valid && g_n1040_debug.node[i].pv_tenths==298);
 }
 char out[2560];assert(n1040_json(out,sizeof(out))>0);puts(out);
 char tiny[8];assert(n1040_json(tiny,sizeof(tiny))==-1 && tiny[0]==0);assert(n1040_json(NULL,0)==-1);
 n1040_init(0);n1040_step(250);uint16_t sensor[]={0,200,68,0,4};respond(sensor,5,260);
 n1040_step(500);respond(pv,1,510);assert(!g_n1040_debug.node[0].valid && g_n1040_debug.node[0].status3==4);
 if(BENCH_N1040_COUNT==1){
  n1040_step(750);respond(status,5,760);assert(!g_n1040_debug.node[0].valid);
  n1040_step(1000);respond(pv,1,1010);assert(g_n1040_debug.node[0].valid);
  uint16_t changed_unit[]={0,200,68,512,0};n1040_step(1250);respond(changed_unit,5,1260);
  assert(!g_n1040_debug.node[0].valid);n1040_step(1500);respond(pv,1,1510);
  assert(g_n1040_debug.node[0].valid && g_n1040_debug.node[0].unit_fahrenheit);
 }
 n1040_init(0);n1040_step(250);uint16_t wrong[]={0,200,99,0,0};respond(wrong,5,260);
 assert(g_n1040_debug.node[0].last_error==9 && !g_n1040_debug.node[0].valid);
 n1040_init(0);n1040_step(250);uint16_t fahrenheit[]={0,200,68,512,0};respond(fahrenheit,5,260);
 n1040_step(500);uint16_t negative[]={0xff9c};respond(negative,1,510);
 assert(g_n1040_debug.node[0].valid && g_n1040_debug.node[0].unit_fahrenheit && g_n1040_debug.node[0].pv_tenths==-100);
 /* Loss of either transaction invalidates the sample; silence never becomes 0 C. */
 n1040_init(0);n1040_step(250);blocked=1;n1040_step(650);
 assert(g_n1040_debug.node[0].timeouts==1 && !g_n1040_debug.node[0].valid);
 n1040_init(0);n1040_step(250);g_rs485_debug.uart_errors++;blocked=1;n1040_step(260);
 assert(g_n1040_debug.node[0].last_error==8 && !g_n1040_debug.waiting);
 n1040_init(0);n1040_step(250);incoming[0]=request[0];incoming[1]=0x83;incoming[2]=2;crc(incoming,3);incoming_size=5;
 n1040_step(260);assert(g_n1040_debug.node[0].exceptions==1 && g_n1040_debug.node[0].exception_code==2);
 n1040_init(0);n1040_step(250);respond(status,5,260);n1040_step(500);respond(pv,1,510);
 blocked=1;n1040_step(6000);assert(!g_n1040_debug.node[0].valid && !g_n1040_debug.node[0].online);
 /* Millisecond wrap: unsigned differences preserve scheduling and timeout. */
 uint32_t start=UINT32_MAX-100;n1040_init(start);n1040_step(start+250);
 assert(g_n1040_debug.waiting);blocked=1;n1040_step(start+650);assert(g_n1040_debug.node[0].timeouts==1);
 /* Worst-case numeric widths still fit the HTTP endpoint's bounded body. */
 memset((void*)&g_n1040_debug,0xff,sizeof(g_n1040_debug));
 for(unsigned i=0;i<BENCH_N1040_COUNT;i++)g_n1040_debug.node[i].pv_tenths=INT32_MIN;
 assert(n1040_json(out,sizeof(out))>0);
 return 0;
}
'''
with tempfile.TemporaryDirectory(prefix='n1040-tests-') as td:
 d=Path(td)
 for nodes in (1,3):
  config=(root/'Application/bench_config.h').read_text().replace('#define BENCH_N1040_COUNT 1U',f'#define BENCH_N1040_COUNT {nodes}U')
  (d/'bench_config.h').write_text(config)
  (d/'n1040.c').write_text((root/'Application/n1040.c').read_text())
  (d/'test.c').write_text(harness)
  exe=d/'test.exe'
  subprocess.run(['gcc','-std=c11','-Wall','-Wextra','-Werror',
   '-I'+str(d),'-I'+str(root/'Application'),str(d/'test.c'),str(d/'n1040.c'),
   str(root/'Application/modbus_read.c'),'-o',str(exe)],check=True)
  result=json.loads(subprocess.check_output([str(exe)],text=True))
  assert result['count']==nodes and len(result['nodes'])==nodes
  assert result['baud']==115200 and result['parity']==0
  assert [n['address'] for n in result['nodes']]==[247,246,245][:nodes]
  assert all(n['valid'] and n['pv_tenths']==298 for n in result['nodes'])
  print(f'PASS: {nodes} node(s): real codec/scheduler, CRC, signed PV, status, ID, Fahrenheit, exceptions, timeout, UART error, stale data, wrap, JSON')
 html=(root/'Application/ui/index.html').read_text(encoding='utf-8')
 js=re.search(r'<script>(.*?)</script>',html,re.S).group(1)
 (d/'web.js').write_text(js,encoding='utf-8')
 subprocess.run(['node','--check',str(d/'web.js')],check=True)
 print('PASS: embedded JavaScript syntax')
