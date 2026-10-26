#include "oled.h"
#include "board.h"
#include "runtime_config.h"
#include "peripheral_io.h"
#include <string.h>
_Static_assert(THERMAL_OLED_WIDTH == OLED_WIDTH && THERMAL_OLED_HEIGHT == OLED_HEIGHT,
               "These profiles only implement 128x64");
static uint8_t frame[1024], pending[1024];
static bool ready, busy, address_needed, display_on;
static unsigned page, column, retries;
static uint32_t retry_ms;
static void oled_try_init(void);
/* Original compact 5x7 uppercase glyph table; lowercase maps to uppercase. */
static const char chars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ-.:/%+!?=>";
static const uint8_t font[][5] = {
 {62,65,65,65,62},{0,66,127,64,0},{98,81,73,73,70},{34,65,73,73,54},
 {24,20,18,127,16},{39,69,69,69,57},{62,73,73,73,48},{1,113,9,5,3},
 {54,73,73,73,54},{6,73,73,41,30},
 {126,17,17,17,126},{127,73,73,73,54},{62,65,65,65,34},{127,65,65,34,28},
 {127,73,73,73,65},{127,9,9,9,1},{62,65,73,73,122},{127,8,8,8,127},
 {0,65,127,65,0},{32,64,65,63,1},{127,8,20,34,65},{127,64,64,64,64},
 {127,2,12,2,127},{127,4,8,16,127},{62,65,65,65,62},{127,9,9,9,6},
 {62,65,81,33,94},{127,9,25,41,70},{38,73,73,73,50},{1,1,127,1,1},
 {63,64,64,64,63},{31,32,64,32,31},{63,64,56,64,63},{99,20,8,20,99},
 {3,4,120,4,3},{97,81,73,69,67},
 {8,8,8,8,8},{0,96,96,0,0},{0,54,54,0,0},{64,32,16,8,4},
 {99,19,8,100,99},{8,8,62,8,8},{0,0,95,0,0},{2,1,81,9,6},
 {20,20,20,20,20},{65,34,20,8,0}
};
void oled_clear(void) { memset(frame, 0, sizeof(frame)); }
const uint8_t *oled_framebuffer(void) { return frame; }
bool oled_ready(void) { return ready; }
bool oled_busy(void) { return busy; }
void oled_pixel(int x, int y, bool on)
{
    if (x < 0 || x >= OLED_WIDTH || y < 0 || y >= OLED_HEIGHT) return;
    unsigned index = (unsigned)x + (unsigned)(y / 8) * OLED_WIDTH;
    uint8_t mask = (uint8_t)(1U << (y % 8));
    if (on) frame[index] |= mask; else frame[index] &= (uint8_t)~mask;
}
void oled_rect(int x, int y, int width, int height, bool fill)
{
    if (width <= 0 || height <= 0 || width > OLED_WIDTH || height > OLED_HEIGHT) return;
    for (int dy = 0; dy < height; ++dy)
        for (int dx = 0; dx < width; ++dx)
            if (fill || dy == 0 || dx == 0 || dy == height-1 || dx == width-1)
                oled_pixel(x+dx, y+dy, true);
}
void oled_text(int x, int y, const char *text)
{
    if (!text) return;
    while (*text && x < OLED_WIDTH) {
        char c = *text++;
        if (c >= 'a' && c <= 'z') c = (char)(c - 'a' + 'A');
        const char *p = c == ' ' ? NULL : strchr(chars, c);
        const uint8_t *glyph = p ? font[p-chars] : NULL;
        for (int dx = 0; dx < 6; ++dx)
            for (int dy = 0; dy < 8; ++dy)
                oled_pixel(x+dx, y+dy, dx < 5 && dy < 7 && glyph &&
                           (glyph[dx] & (1U << dy)) != 0U);
        x += 6;
    }
}
void oled_bitmap(int x, int y, unsigned w, unsigned h, const uint8_t *data, size_t size)
{
    if (!data || w == 0 || w > OLED_WIDTH || h == 0 || h > OLED_HEIGHT) return;
    unsigned stride = (w + 7U) / 8U;
    if (size < (size_t)stride * h) return;
    for (unsigned dy = 0; dy < h; ++dy)
        for (unsigned dx = 0; dx < w; ++dx)
            oled_pixel(x+(int)dx, y+(int)dy,
                       (data[dy*stride + dx/8U] & (0x80U >> (dx%8U))) != 0);
}
void oled_text_big(int x, int y, const char *text)
{
    if (!text) return;
    while (*text && x < OLED_WIDTH) {
        char c = *text++;
        if (c >= 'a' && c <= 'z') c = (char)(c-'a'+'A');
        const char *p = c == ' ' ? NULL : strchr(chars,c);
        const uint8_t *glyph = p ? font[p-chars] : NULL;
        for (int dx = 0; dx < 6; ++dx) for (int dy = 0; dy < 8; ++dy) {
            bool on = dx < 5 && dy < 7 && glyph && (glyph[dx] & (1U<<dy));
            for (int sx = 0; sx < 2; ++sx) for (int sy = 0; sy < 2; ++sy)
                oled_pixel(x+dx*2+sx,y+dy*2+sy,on);
        }
        x += 12;
    }
}
void oled_xbm(int x, int y, unsigned w, unsigned h, const uint8_t *data, size_t size)
{
    if (!data || w == 0 || w > OLED_WIDTH || h == 0 || h > OLED_HEIGHT) return;
    unsigned stride = (w+7U)/8U;
    if (size < (size_t)stride*h) return;
    for (unsigned dy = 0; dy < h; ++dy) for (unsigned dx = 0; dx < w; ++dx)
        oled_pixel(x+(int)dx,y+(int)dy,(data[dy*stride+dx/8U] & (1U<<(dx%8U))) != 0);
}
bool oled_present(void)
{
    if (!ready || busy) return false;
    memcpy(pending, frame, sizeof(frame));
    page = column = 0; address_needed = true; busy = true;
    return true;
}
void oled_service(void)
{
    if (!ready) return;
    if (!busy) {
        if (!display_on) {
            const uint8_t on[] = {0, 0xAF};
            if (!board_oled_write(on, sizeof(on))) { ready = false; retry_ms = board_millis(); return; }
            display_on = true;
        }
        return;
    }
    uint8_t message[17];
    size_t length;
    if (address_needed) {
        message[0] = 0; message[1] = (uint8_t)(0xB0U + page);
        unsigned offset = board_oled_column_offset();
        message[2] = (uint8_t)(offset & 15U);
        message[3] = (uint8_t)(0x10U | (offset >> 4)); length = 4;
    } else {
        message[0] = 0x40;
        memcpy(message+1, pending + page*128U + column, 16);
        length = 17;
    }
    if (!board_oled_write(message, length)) { ready = busy = false; retry_ms = board_millis(); return; }
    if (address_needed) { address_needed = false; return; }
    column += 16;
    if (column == 128) {
        column = 0; address_needed = true;
        if (++page == 8) busy = false;
    }
}
static void oled_try_init(void)
{
    ready = busy = display_on = false; oled_clear();
    if (!THERMAL_OLED_CONFIRMED) return;
    if (!board_oled_prepare()) return;
    /* SSD1306 electrical settings matched to supplied 0.96-inch example.
     * Use documented PAGE addressing 0x02 (not the example's 0x10 parameter).
     * SH1106 uses its own DC/DC command and RAM column offset.
     * Each command/parameter is sent separately like the supplied example. */
    static const uint8_t ssd1306[] = {0xAE,0x20,0x02,0xB0,0xC8,0x00,0x10,
        0x40,0x81,0xFF,0xA1,0xA6,0xA8,0x3F,0xA4,0xD3,0x00,0xD5,0xF0,
        0xD9,0x22,0xDA,0x12,0xDB,0x20,0x8D,0x14,0x2E};
    static const uint8_t sh1106[] = {0xAE,0xD5,0x80,0xA8,0x3F,
        0xD3,0x00,0x40,0xAD,0x8B,0xA1,0xC8,0xDA,0x12,
        0x81,0x7F,0xD9,0x22,0xDB,0x35,0xA4,0xA6};
    const uint8_t *commands;
    size_t count;
    uint8_t controller = board_oled_controller();
    if (controller == OLED_DRIVER_SSD1306) {
        commands = ssd1306; count = sizeof(ssd1306);
    } else if (controller == OLED_DRIVER_SH1106) {
        commands = sh1106; count = sizeof(sh1106);
    } else return;
    for (size_t i=0;i<count;++i) {
        const uint8_t command[] = {0,commands[i]};
        if (!board_oled_write(command,sizeof(command))) return;
    }
    /* Clear display RAM before display-on to avoid random splash pixels. */
    ready = true;
    (void)oled_present();
}

void oled_init(void)
{
    retries = 0;
    oled_try_init();
    retry_ms = board_millis();
}

/* May block during power-up settling; call only while no thermal job is active. */
void oled_recover_service(void)
{
    if (!ready && retries < 3U && (uint32_t)(board_millis()-retry_ms) >= 1000U) {
        retries++; oled_try_init(); retry_ms = board_millis();
    }
}
