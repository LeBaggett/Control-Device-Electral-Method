#include "vf_math.h"
#include <math.h>
float vf_from_counts(float average, float vdda)
{
    return isfinite(average) && average >= 0 && average <= 4095 &&
        isfinite(vdda) && vdda >= 1.8f && vdda <= 3.6f ? average*vdda/4095.0f : NAN;
}
float vdda_from_vrefint(uint16_t calibration, uint16_t raw)
{
    if (calibration == 0 || calibration > 4095 || raw == 0 || raw > 4095) return NAN;
    float vdda = 3.3f*calibration/raw;
    return vdda >= 1.8f && vdda <= 3.6f ? vdda : NAN;
}
