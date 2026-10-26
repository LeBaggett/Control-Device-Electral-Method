#ifndef THERMAL_RUNTIME_CONFIG_H
#define THERMAL_RUNTIME_CONFIG_H
/* Display and direct Vf conversion match the owner's attached plan.
 * These settings do not unlock relay power; board lock stays in place. */
#ifndef THERMAL_OLED_CONFIRMED
#define THERMAL_OLED_CONFIRMED 1
#endif
#define THERMAL_OLED_WIDTH 128
#define THERMAL_OLED_HEIGHT 64
/* Controller selection and SH1106 offset are centralized in pinout.h. */
#define THERMAL_ENCODER_COUNTS_PER_DETENT 2
#define THERMAL_ENCODER_DIRECTION 1
#define THERMAL_BUTTON_DEBOUNCE_MS 30U
/* Mechanical relay closure/current settling, before ADC or heating. */
#define THERMAL_RELAY_SETTLE_MS 100U
#define THERMAL_DISPLAY_FILTER_ALPHA 0.25f
#include "config.h"
#define THERMAL_BUTTON_LONG_MS LONG_PRESS_MS
#ifndef THERMAL_ADC_CONFIRMED
#define THERMAL_ADC_CONFIRMED 1
#endif
#define THERMAL_ADC_VREF_V VDDA_NOMINAL_V
/* Vpin = gain * Vf + offset; confirm from schematic/calibration. */
#define THERMAL_ADC_GAIN 1.0f
#define THERMAL_ADC_OFFSET_V 0.0f
#define THERMAL_ADC_AVERAGE_COUNT 16U
#ifndef THERMAL_USE_FATFS
#define THERMAL_USE_FATFS 1
#endif
/* Temporary diagnostic: skip 4-bit negotiation; restore 4U after verification. */
#ifndef THERMAL_SD_BUS_BITS
#define THERMAL_SD_BUS_BITS 1U
#endif
#ifndef THERMAL_SD_CLOCK_DIV
#define THERMAL_SD_CLOCK_DIV 118U /* Diagnostic: 48 MHz/(118+2) = 400 kHz. */
#endif
#define THERMAL_SD_POWERUP_MS 300U
#define THERMAL_SD_VOLUME "0:"
/* UI defaults from the attached plan. Physical verification pending. */
#define THERMAL_CAL_START_C 25.0f
#define THERMAL_CAL_END_C 85.0f
#define THERMAL_CAL_STEP_C 10.0f
#define THERMAL_UI_MIN_C TEMP_MIN_C
#define THERMAL_UI_MAX_C TEMP_MAX_C
/* Assisted thermocouple calibration; tune only after bench verification. */
#define TC_HEAT_MIN_C 25.0f
#define TC_HEAT_MAX_C 80.0f
#define TC_HEAT_MIN_STEP_C 5.0f
/* Latest converted sample at the operator click; no delayed averaging. */
#define TC_HEAT_CONFIRM_MAX_AGE_MS 300U
#define TC_HEAT_SENSOR_MAX_AGE_MS 1000U
#define TC_HEAT_POINT_TIMEOUT_MS 600000U
#define TC_HEAT_OVERSHOOT_C 10.0f
#endif
