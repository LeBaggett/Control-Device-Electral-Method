#include "max31855.h"
#include "hw_debug.h"
#include <math.h>
#include "board.h"
void max31855_init(void) {
 g_hw_debug.tc_valid = false;
 g_hw_debug.tc_raw_c = g_hw_debug.tc_internal_c = g_hw_debug.tc_display_c = NAN;
 g_hw_debug.tc_min_c = g_hw_debug.tc_max_c = NAN;
}
max31855_status_t max31855_decode(uint32_t raw, float *tc, float *internal)
{
    int32_t t14, t12;
    if (tc == 0) return MAX31855_FRAME_ERROR;
    /* Bits reservados 17 e 3 devem ser zero. */
    if ((raw & ((1UL << 17) | (1UL << 3))) != 0U)
        return MAX31855_FRAME_ERROR;
    if ((raw & (1UL << 16)) != 0U) {
        if ((raw & 1U) != 0U) return MAX31855_OPEN_CIRCUIT;
        if ((raw & 2U) != 0U) return MAX31855_SHORT_GND;
        if ((raw & 4U) != 0U) return MAX31855_SHORT_VCC;
        return MAX31855_FRAME_ERROR;
    }
    if ((raw & 7U) != 0U) return MAX31855_FRAME_ERROR;
    t14 = (int32_t)((raw >> 18) & 0x3FFFU);
    if ((t14 & 0x2000) != 0) t14 -= 0x4000;
    t12 = (int32_t)((raw >> 4) & 0x0FFFU);
    if ((t12 & 0x0800) != 0) t12 -= 0x1000;
    *tc = (float)t14 * 0.25f;
    if (internal != 0) *internal = (float)t12 * 0.0625f;
    return MAX31855_OK;
}
max31855_status_t max31855_read(float *thermocouple_c)
{
 uint32_t raw = 0;
 float internal = NAN;
 max31855_status_t status = MAX31855_SPI_ERROR;
 if (thermocouple_c && board_max31855_read32(&raw))
     status = max31855_decode(raw, thermocouple_c, &internal);
 g_hw_debug.tc_frame = raw;
 g_hw_debug.tc_status = status;
 g_hw_debug.tc_samples++;
 if (status != MAX31855_OK) {
     g_hw_debug.tc_valid = false;
     g_hw_debug.tc_errors++;
     g_hw_debug.tc_raw_c = g_hw_debug.tc_internal_c = g_hw_debug.tc_display_c = NAN;
     return status;
 }
 float value = *thermocouple_c;
 float alpha = g_hw_tuning.display_filter_alpha;
 if (!(alpha > 0.0f && alpha <= 1.0f)) alpha = 1.0f;
 g_hw_debug.tc_display_c = !g_hw_debug.tc_valid ? value :
     g_hw_debug.tc_display_c + alpha * (value-g_hw_debug.tc_display_c);
 g_hw_debug.tc_raw_c = value; g_hw_debug.tc_internal_c = internal;
 if (!isfinite(g_hw_debug.tc_min_c) || value < g_hw_debug.tc_min_c) g_hw_debug.tc_min_c = value;
 if (!isfinite(g_hw_debug.tc_max_c) || value > g_hw_debug.tc_max_c) g_hw_debug.tc_max_c = value;
 g_hw_debug.tc_valid = true; g_hw_debug.tc_last_ok_ms = board_millis();
 return status; /* Control and logging still receive the unfiltered sample. */
}
