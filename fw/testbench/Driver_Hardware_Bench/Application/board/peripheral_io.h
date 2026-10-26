#ifndef PERIPHERAL_IO_H
#define PERIPHERAL_IO_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#define OLED_DRIVER_SSD1306 1U
#define OLED_DRIVER_SH1106 2U
uint8_t board_oled_controller(void);
uint8_t board_oled_column_offset(void);
bool board_oled_prepare(void);
bool board_oled_write(const uint8_t *bytes, size_t count);
bool board_encoder_start(uint32_t *counter);
uint32_t board_encoder_counter(void);
bool board_encoder_button_pressed(void);
bool board_adc_start(void);
bool board_adc_sample(uint16_t *value);
bool board_adc_begin(bool vrefint);
void board_adc_cancel(void);
uint16_t board_vrefint_calibration(void);
bool board_adc_burst(unsigned count, float *average);
#endif
