#ifndef BENCH_CONFIG_H
#define BENCH_CONFIG_H
#define BENCH_VERSION "0.1.0-base-ethernet"
#define BENCH_HEARTBEAT_MS 500U
#define BENCH_LINK_POLL_MS 250U
#define BENCH_IP_A 192
#define BENCH_IP_B 168
#define BENCH_IP_C 7
#define BENCH_IP_D 2
#define BENCH_POWER_ENABLED 0
#if BENCH_POWER_ENABLED != 0
#error "This bring-up firmware has no power-control implementation"
#endif
#endif
