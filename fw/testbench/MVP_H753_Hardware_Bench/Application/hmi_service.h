#ifndef BENCH_HMI_SERVICE_H
#define BENCH_HMI_SERVICE_H
#include <stdint.h>
#include <stddef.h>
#define HMI_REGISTER_COUNT 64U
typedef struct {uint32_t requests,responses,exceptions,crc_errors,bad_frames,ignored,tx_failures,last_request_ms;} hmi_debug_t;
extern volatile hmi_debug_t g_hmi_debug;
void hmi_init(uint32_t now);
void hmi_step(uint32_t now);
void hmi_register_snapshot(uint16_t registers[HMI_REGISTER_COUNT],uint32_t now);
int hmi_json(char *out,size_t capacity);
#endif
