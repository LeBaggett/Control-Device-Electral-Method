#ifndef BENCH_ETHERNETIF_H
#define BENCH_ETHERNETIF_H
#include "lwip/netif.h"
err_t ethernetif_init(struct netif *netif);
void ethernetif_input(struct netif *netif);
void ethernet_link_check_state(struct netif *netif);
#endif
