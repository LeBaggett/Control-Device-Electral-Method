#include "rtd.h"
#include "max31855.h"
#include "tc_calibration.h"
/* Compatibilidade temporária com a API anterior; sensor agora é termopar. */
void rtd_init(void) { max31855_init(); }
bool rtd_read_temperature(float *temperature_c)
{
    if (max31855_read(temperature_c) != MAX31855_OK) return false;
    *temperature_c = tc_calibration_apply(*temperature_c);
    return true;
}
