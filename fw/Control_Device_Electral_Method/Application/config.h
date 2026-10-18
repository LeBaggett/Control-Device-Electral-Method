#ifndef THERMAL_CONFIG_H
#define THERMAL_CONFIG_H
/* Protocol values from the attached plan, override with -D for host tests. */
#ifndef TEMP_MIN_C
#define TEMP_MIN_C 0.0f
#endif
#ifndef TEMP_MAX_C
#define TEMP_MAX_C 150.0f
#endif
#ifndef OVERTEMP_C
#define OVERTEMP_C 160.0f
#endif
#ifndef TARGETS_MAX
#define TARGETS_MAX 200U
#endif
/* Legacy tolerance/soak values retained for compatibility; immediate capture does not use them. */
#ifndef CAL_TOL_C
#define CAL_TOL_C 0.5f
#endif
#ifndef CAL_SOAK_S
#define CAL_SOAK_S 180U
#endif
#ifndef CAL_WAIT_TIMEOUT_S
#define CAL_WAIT_TIMEOUT_S 3600U
#endif
#ifndef CAL_MEAS_DELAY_US
#define CAL_MEAS_DELAY_US 30U
#endif
#ifndef CAL_VF_SAMPLES
#define CAL_VF_SAMPLES 64U
#endif
#ifndef CAL_MIN_POINTS_WARN
#define CAL_MIN_POINTS_WARN 5U
#endif
#ifndef TEST_MEAS_DELAY_US
#define TEST_MEAS_DELAY_US 30U
#endif
#ifndef TEST_VF_SAMPLES
#define TEST_VF_SAMPLES 8U
#endif
#ifndef TEST_TARGET_TIMEOUT_S
#define TEST_TARGET_TIMEOUT_S 600U
#endif
#ifndef RTD_FAULT_LIMIT
#define RTD_FAULT_LIMIT 3U
#endif
#ifndef REQUIRE_CAL
#define REQUIRE_CAL 0
#endif
#ifndef VDDA_NOMINAL_V
#define VDDA_NOMINAL_V 3.3f
#endif
#ifndef SPLASH_MS
#define SPLASH_MS 2000U
#endif
#ifndef LONG_PRESS_MS
#define LONG_PRESS_MS 1000U
#endif
#endif
