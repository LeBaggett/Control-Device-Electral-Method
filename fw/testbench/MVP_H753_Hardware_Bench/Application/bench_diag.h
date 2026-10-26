#ifndef BENCH_DIAG_H
#define BENCH_DIAG_H
#include <stdint.h>
#include <stddef.h>
typedef struct {
 uint32_t magic,boot_stage,fatal_code,core_hz,hclk_hz,uptime_ms,loops,max_gap_ms;
 uint32_t led_toggles,link_up,link_changes,http_requests,http_alloc_failures;
 uint32_t rx_frames,rx_irq,tx_frames,tx_irq,tx_failures,eth_irq_errors;
 uint32_t eth_hal_status,eth_error,dma_error,phy_status,phy_address;
 uint32_t cfsr,hfsr,bfar,mmfar;
 uint8_t mac[6];
} bench_diag_t;
extern volatile bench_diag_t g_bench_debug;
void bench_diag_init(uint32_t now);
void bench_diag_step(uint32_t now);
void bench_diag_link(uint32_t up);
int bench_diag_json(char *out,size_t capacity);
#endif
