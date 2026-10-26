#ifndef SDLOG_H
#define SDLOG_H

#include <stdbool.h>
#include <stdint.h>
extern volatile uint32_t g_sd_filesystem_type; /* 0=unmounted, 1=FAT12, 2=FAT16, 3=FAT32, 4=exFAT */
#include <stddef.h>
#include "linfit.h"
void sdlog_init(void);
bool sdlog_ready(void);
bool sdlog_open_run(bool calibration);
bool sdlog_line(const char *line);
bool sdlog_close(void);
const char *sdlog_run_name(void);
bool sdlog_save_calibration(const cal_params_t *calibration);
bool sdlog_load_calibration(cal_params_t *calibration);

#endif
