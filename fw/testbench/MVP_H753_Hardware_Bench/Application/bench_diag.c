#include "bench_diag.h"
#include "bench_config.h"
#include <stdio.h>
#include <string.h>
volatile bench_diag_t g_bench_debug;
static uint32_t previous;
void bench_diag_init(uint32_t now) {
 memset((void *)&g_bench_debug,0,sizeof(g_bench_debug));
 g_bench_debug.magic=0x48373533U;previous=now;
}
void bench_diag_step(uint32_t now) {
 /* Unsigned subtraction also handles the 32-bit millisecond counter wrap. */
 uint32_t gap=now-previous;previous=now;
 g_bench_debug.uptime_ms=now;
 if(g_bench_debug.loops && gap>g_bench_debug.max_gap_ms)g_bench_debug.max_gap_ms=gap;
 g_bench_debug.loops++;
}
void bench_diag_link(uint32_t up) {
 up=up!=0U;
 if(up!=g_bench_debug.link_up) {g_bench_debug.link_up=up;g_bench_debug.link_changes++;}
}
int bench_diag_json(char *out,size_t capacity) {
 if(!out || !capacity)return -1;
 /* Only bounded integers and fixed strings; no dynamic/unescaped input. */
 int n=snprintf(out,capacity,
 "{\"version\":\"" BENCH_VERSION "\",\"power_enabled\":false,"
 "\"uptime_ms\":%lu,\"loops\":%lu,\"max_gap_ms\":%lu,"
 "\"core_hz\":%lu,\"hclk_hz\":%lu,\"boot_stage\":%lu,\"fatal_code\":%lu,"
 "\"link_up\":%lu,\"link_changes\":%lu,\"http_requests\":%lu,"
 "\"rx_frames\":%lu,\"tx_frames\":%lu,\"tx_failures\":%lu,"
 "\"eth_error\":%lu,\"dma_error\":%lu,\"phy_status\":%lu}",
 (unsigned long)g_bench_debug.uptime_ms,(unsigned long)g_bench_debug.loops,
 (unsigned long)g_bench_debug.max_gap_ms,(unsigned long)g_bench_debug.core_hz,
 (unsigned long)g_bench_debug.hclk_hz,(unsigned long)g_bench_debug.boot_stage,
 (unsigned long)g_bench_debug.fatal_code,(unsigned long)g_bench_debug.link_up,
 (unsigned long)g_bench_debug.link_changes,(unsigned long)g_bench_debug.http_requests,
 (unsigned long)g_bench_debug.rx_frames,(unsigned long)g_bench_debug.tx_frames,
 (unsigned long)g_bench_debug.tx_failures,(unsigned long)g_bench_debug.eth_error,
 (unsigned long)g_bench_debug.dma_error,(unsigned long)g_bench_debug.phy_status);
 if(n<0 || (size_t)n>=capacity){out[0]=0;return -1;}return n;
}
