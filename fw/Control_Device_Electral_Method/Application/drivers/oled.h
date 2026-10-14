#ifndef OLED_H
#define OLED_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
void oled_init(void);
bool oled_ready(void);
void oled_clear(void);
void oled_pixel(int x, int y, bool on);
void oled_rect(int x, int y, int width, int height, bool fill);
void oled_text(int x, int y, const char *text);
void oled_text_big(int x, int y, const char *text);
void oled_xbm(int x, int y, unsigned width, unsigned height,
              const uint8_t *data, size_t size);
/* Row-major monochrome, MSB left; clipped to display. */
void oled_bitmap(int x, int y, unsigned width, unsigned height,
                 const uint8_t *data, size_t size);
/* Snapshot a rendered frame; false when another frame is in transit. */
bool oled_present(void);
/* One small I2C transaction per call, maximum HAL timeout 5 ms. */
void oled_service(void);
bool oled_busy(void);
const uint8_t *oled_framebuffer(void);

#endif
