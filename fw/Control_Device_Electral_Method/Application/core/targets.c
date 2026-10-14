#include "targets.h"
#include "config.h"
#include <math.h>
size_t targets_count(const sweep_cfg_t *c)
{
    if (!c || !isfinite(c->t_start_c) || !isfinite(c->t_end_c) ||
        !isfinite(c->t_step_c) || c->t_step_c <= 0 || c->t_end_c <= c->t_start_c ||
        c->t_start_c < TEMP_MIN_C || c->t_end_c > TEMP_MAX_C) return 0;
    double intervals = ((double)c->t_end_c-c->t_start_c)/c->t_step_c;
    if (!isfinite(intervals) || intervals > TARGETS_MAX) return 0;
    size_t count = (size_t)floor(intervals + 1e-4) + 1;
    return count >= 1 && count <= TARGETS_MAX ? count : 0;
}
float targets_at(const sweep_cfg_t *c, size_t index)
{
    size_t count = targets_count(c);
    if (index >= count) return NAN;
    float value = (float)((double)c->t_start_c+index*(double)c->t_step_c);
    return value <= c->t_end_c + 0.001f ? value : NAN;
}
