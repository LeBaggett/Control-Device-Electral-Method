#include "calibration.h"
#include "sdlog.h"
static cal_params_t saved;
void calibration_init(void)
{
    saved = (cal_params_t){0}; (void)sdlog_load_calibration(&saved);
}
const cal_params_t *calibration_get(void) { return &saved; }
void calibration_set(const cal_params_t *cal) { if (cal) saved = *cal; }
