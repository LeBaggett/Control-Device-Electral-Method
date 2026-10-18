#ifndef THERMAL_SD_IO_H
#define THERMAL_SD_IO_H
#include <stdbool.h>
#include <stdint.h>
/* Independent of g_hw_debug: retained results survive later status polling. */
typedef struct {
 uint32_t read_attempts, read_hal_status, read_hal_error, read_sector, read_blocks;
 uint32_t read_clkcr, read_cmd, read_response1;
 uint32_t last_card_state, card_status_hal_error;
 uint32_t card_type, card_logical_blocks;
 uint32_t first_read_signature;
 uint32_t read_retries,read_crc_failures,last_failed_read_error;
} sd_io_debug_t;
extern volatile sd_io_debug_t g_sd_io_debug;
bool board_sd_prepare(void);
bool board_sd_is_ready(void);
#endif
