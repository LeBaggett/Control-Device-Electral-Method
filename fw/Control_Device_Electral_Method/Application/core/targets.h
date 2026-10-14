#ifndef TARGETS_H
#define TARGETS_H
#include <stddef.h>
typedef struct { float t_start_c, t_end_c, t_step_c; } sweep_cfg_t;
size_t targets_count(const sweep_cfg_t *cfg);
float targets_at(const sweep_cfg_t *cfg, size_t index);
#endif
