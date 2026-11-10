#ifndef BENCH_CONFIG_H
#define BENCH_CONFIG_H
#define BENCH_VERSION "0.2.2-n1040i"
/* N1040i menu must match. Parity: 0 none, 1 even, 2 odd (8 data + 1 stop). */
#define BENCH_N1040_ENABLED 1
/* 0: automatic-direction module (VCC/TX/RX/GND); 1: PD4 drives DE+/RE. */
#define BENCH_RS485_MANUAL_DIRECTION 0U
#if BENCH_RS485_MANUAL_DIRECTION != 0 && BENCH_RS485_MANUAL_DIRECTION != 1
#error "RS485 direction must be automatic (0) or manual (1)"
#endif
#define BENCH_RS485_BAUD 115200U
#define BENCH_RS485_PARITY 0U
#define BENCH_N1040_COUNT 1U
#define BENCH_N1040_ADDR1 247U
#define BENCH_N1040_ADDR2 246U
#define BENCH_N1040_ADDR3 245U
#define BENCH_N1040_POLL_MS 250U
#define BENCH_N1040_TIMEOUT_MS 400U
#define BENCH_N1040_STALE_MS 5000U
#if BENCH_N1040_COUNT < 1 || BENCH_N1040_COUNT > 3 || BENCH_RS485_PARITY > 2
#error "Invalid N1040i node count or parity"
#endif
#if BENCH_RS485_BAUD < 1200U || BENCH_RS485_BAUD > 115200U
#error "RS485 baud outside supported bench range"
#endif
#if BENCH_N1040_ADDR1 < 1 || BENCH_N1040_ADDR1 > 247 || BENCH_N1040_ADDR2 < 1 || BENCH_N1040_ADDR2 > 247 || BENCH_N1040_ADDR3 < 1 || BENCH_N1040_ADDR3 > 247
#error "Modbus slave addresses must be 1..247"
#endif
#if BENCH_N1040_COUNT > 1 && BENCH_N1040_ADDR1 == BENCH_N1040_ADDR2
#error "Duplicate Modbus addresses"
#endif
#if BENCH_N1040_COUNT > 2 && (BENCH_N1040_ADDR3 == BENCH_N1040_ADDR1 || BENCH_N1040_ADDR3 == BENCH_N1040_ADDR2)
#error "Duplicate Modbus addresses"
#endif
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
