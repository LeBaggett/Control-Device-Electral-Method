#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "sd_io.h"
volatile sd_io_debug_t g_sd_io_debug;
int main(void){
 uint8_t mbr[512]={0},boot[512]={0};
 mbr[510]=0x55;mbr[511]=0xAA;mbr[450]=0x0C;mbr[455]=8;
 sd_capture_sector(0,mbr);
 assert(g_sd_io_debug.sector_trace_count==1);
 assert(g_sd_io_debug.sector_trace[0].signature==0xAA55);
 assert(g_sd_io_debug.sector_trace[0].first_partition_lba==2048);
 boot[0]=0xEB;boot[2]=0x90;boot[12]=2;boot[13]=8;boot[14]=32;boot[16]=2;
 boot[510]=0x55;boot[511]=0xAA;boot[34]=8;boot[36]=100;boot[44]=2;
 memcpy(boot+82,"FAT32   ",8);sd_capture_sector(2048,boot);
 assert(g_sd_io_debug.sector_trace[1].bytes_per_sector==512);
 assert(g_sd_io_debug.sector_trace[1].total_sectors==524288);
 assert(g_sd_io_debug.sector_trace[1].fat_sectors==100);
 assert(g_sd_io_debug.sector_trace[1].root_cluster==2);
 for(unsigned i=0;i<8;i++)assert(g_sd_io_debug.sector_trace[1].fat_type[i]==(uint8_t)"FAT32   "[i]);
 sd_capture_sector(2048,mbr);assert(g_sd_io_debug.sector_trace_count==2);
 sd_capture_sector(10,boot);sd_capture_sector(11,boot);sd_capture_sector(12,boot);
 assert(g_sd_io_debug.sector_trace_count==4);
 puts("SD sector trace: MBR/FAT32 parsing, duplicate retention and cap PASS");return 0;
}
