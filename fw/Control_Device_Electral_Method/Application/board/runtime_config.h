#ifndef THERMAL_RUNTIME_CONFIG_H
#define THERMAL_RUNTIME_CONFIG_H
/* Display and direct Vf conversion match the owner's attached plan.
 * These settings do not unlock relay power; board lock stays in place. */
#ifndef THERMAL_OLED_CONFIRMED
#define THERMAL_OLED_CONFIRMED 1
#endif
#define THERMAL_OLED_WIDTH 128
#define THERMAL_OLED_HEIGHT 64
#define THERMAL_OLED_SSD1306 1
#define THERMAL_OLED_SH1106 2
#ifndef THERMAL_OLED_CONTROLLER
#define THERMAL_OLED_CONTROLLER THERMAL_OLED_SH1106
#endif
/* SH1106 modules commonly expose 128 of 132 columns, at RAM offset 2.
 * Confirm resolution, offset, supply/reset and orientation on actual module. */
#define THERMAL_OLED_COLUMN_OFFSET 2U
#define THERMAL_ENCODER_COUNTS_PER_DETENT 4
#define THERMAL_ENCODER_DIRECTION 1
#define THERMAL_BUTTON_DEBOUNCE_MS 30U
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
#define THERMAL_SD_VOLUME "0:"
/* UI defaults from the attached plan. Physical verification pending. */
#define THERMAL_CAL_START_C 25.0f
#define THERMAL_CAL_END_C 85.0f
#define THERMAL_CAL_STEP_C 10.0f
#define THERMAL_UI_MIN_C TEMP_MIN_C
#define THERMAL_UI_MAX_C TEMP_MAX_C
#endif
