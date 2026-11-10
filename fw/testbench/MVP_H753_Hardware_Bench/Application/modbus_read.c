#include "modbus_read.h"
uint16_t mb_crc16(const uint8_t *data,size_t len){
 uint16_t crc=0xffffU;
 for(size_t i=0;i<len;i++){crc^=data[i];for(unsigned j=0;j<8;j++)crc=(crc&1U)?(uint16_t)((crc>>1)^0xa001U):(uint16_t)(crc>>1);}
 return crc;
}
size_t mb_read_request(uint8_t out[8],uint8_t address,uint16_t reg,uint16_t count){
 if(!out || address<1 || address>247 || !count || count>125)return 0;
 out[0]=address;out[1]=3;out[2]=(uint8_t)(reg>>8);out[3]=(uint8_t)reg;
 out[4]=(uint8_t)(count>>8);out[5]=(uint8_t)count;
 uint16_t crc=mb_crc16(out,6);out[6]=(uint8_t)crc;out[7]=(uint8_t)(crc>>8);return 8;
}
mb_result_t mb_read_response(const uint8_t *d,size_t n,uint8_t address,
 uint16_t count,uint16_t *words,uint8_t *exception){
 if(exception)*exception=0;
 if(!d || n<5)return MB_SHORT;
 if(mb_crc16(d,n-2)!=((uint16_t)d[n-2]|((uint16_t)d[n-1]<<8)))return MB_CRC;
 if(d[0]!=address)return MB_ADDRESS;
 if(d[1]==0x83){if(n!=5)return MB_LENGTH;if(exception)*exception=d[2];return MB_EXCEPTION;}
 if(d[1]!=3)return MB_FUNCTION;
 if(!count || count>125 || !words || n!=5U+2U*count || d[2]!=2U*count)return MB_LENGTH;
 for(unsigned i=0;i<count;i++)words[i]=((uint16_t)d[3+2*i]<<8)|d[4+2*i];
 return MB_OK;
}
int32_t mb_signed_word(uint16_t word){return word&0x8000U?(int32_t)word-65536:(int32_t)word;}
