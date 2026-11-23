#include "modbus_slave.h"
#include "modbus_read.h"
static size_t finish(uint8_t *reply,size_t length){
 uint16_t crc=mb_crc16(reply,length);reply[length]=(uint8_t)crc;reply[length+1]=(uint8_t)(crc>>8);return length+2;
}
size_t mb_slave_read(const uint8_t *q,size_t n,uint8_t address,const uint16_t *regs,
 size_t count,uint8_t *reply,size_t capacity,slave_result_t *result){
 if(!result)return 0;
 *result=SLAVE_IGNORED;
 if(!q || !n || address==0 || address>247 || q[0]!=address)return 0;
 /* Broadcasts/wrong addresses are silent. Never answer an exception/response
  * echoed by automatic transceivers, avoiding reply loops. */
 if(n<4){*result=SLAVE_BAD_FRAME;return 0;}
 if(q[1]&0x80U)return 0;
 if(mb_crc16(q,n-2)!=((uint16_t)q[n-2]|((uint16_t)q[n-1]<<8))){*result=SLAVE_CRC_ERROR;return 0;}
 if(!reply || capacity<5 || !regs){*result=SLAVE_BAD_FRAME;return 0;}
 unsigned exception=0;
 if(q[1]!=3 && q[1]!=4)exception=1; /* Read-only bench. */
 else{
  if(n!=8){*result=SLAVE_BAD_FRAME;return 0;}
  uint16_t first=((uint16_t)q[2]<<8)|q[3],quantity=((uint16_t)q[4]<<8)|q[5];
  if(quantity==0 || quantity>64)exception=3;
  else if(first>=count || quantity>count-first)exception=2;
  else if(capacity<5U+2U*quantity){*result=SLAVE_BAD_FRAME;return 0;}
  else{
   reply[0]=address;reply[1]=q[1];reply[2]=(uint8_t)(quantity*2U);
   for(unsigned i=0;i<quantity;i++){uint16_t value=regs[first+i];reply[3+2*i]=(uint8_t)(value>>8);reply[4+2*i]=(uint8_t)value;}
   *result=SLAVE_OK;return finish(reply,3U+2U*quantity);
  }
 }
 reply[0]=address;reply[1]=(uint8_t)(q[1]|0x80U);reply[2]=(uint8_t)exception;
 *result=SLAVE_EXCEPTION;return finish(reply,3);
}
