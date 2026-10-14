#include "max31855.h"
#include "board.h"
void max31855_init(void) { /* Conversor somente leitura, sem registradores. */ }
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
    uint32_t raw;
    if (thermocouple_c == 0 || !board_max31855_read32(&raw))
        return MAX31855_SPI_ERROR;
    return max31855_decode(raw, thermocouple_c, 0);
}
