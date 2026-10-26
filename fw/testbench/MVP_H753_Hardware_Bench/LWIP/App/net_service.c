#include "net_service.h"
#include "main.h"
#include "bench_config.h"
#include "bench_diag.h"
#include "stm32h7xx_hal.h"
#include "ethernetif.h"
#include "lwip/init.h"
#include "lwip/timeouts.h"
#include "lwip/apps/httpd.h"
extern ETH_HandleTypeDef EthHandle;
static struct netif iface;
static uint32_t link_checked;
void bench_net_init(void) {
 ip4_addr_t ip,mask,gateway;
 IP4_ADDR(&ip,BENCH_IP_A,BENCH_IP_B,BENCH_IP_C,BENCH_IP_D);
 IP4_ADDR(&mask,255,255,255,0);IP4_ADDR(&gateway,0,0,0,0);
 /* Locally administered unicast MAC, derived from device UID for bench use. */
 uint32_t uid=HAL_GetUIDw0()^HAL_GetUIDw1()^HAL_GetUIDw2();
 g_bench_debug.mac[0]=2;g_bench_debug.mac[1]=0xCA;
 for(unsigned i=0;i<4;i++)g_bench_debug.mac[i+2]=(uint8_t)(uid>>(i*8));
 lwip_init();g_bench_debug.boot_stage=4;
 if(!netif_add(&iface,&ip,&mask,&gateway,0,ethernetif_init,netif_input))bench_fatal(10);
 netif_set_default(&iface);httpd_init();link_checked=HAL_GetTick();
 g_bench_debug.boot_stage=5;
}
void bench_net_step(void) {
 /* lwIP/raw and pbuf operations run only here/mainloop, never in ETH ISR. */
 (void)HAL_ETH_ReleaseTxPacket(&EthHandle);
 ethernetif_input(&iface);sys_check_timeouts();
 uint32_t now=HAL_GetTick();
 if((uint32_t)(now-link_checked)>=BENCH_LINK_POLL_MS){link_checked=now;ethernet_link_check_state(&iface);}
 bench_diag_link(netif_is_link_up(&iface));
}
