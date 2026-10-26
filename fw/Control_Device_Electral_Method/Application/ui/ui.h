#ifndef UI_H
#define UI_H
#include "device.h"
#include "sofia.h"
#include "input_event.h"

typedef enum {
    UI_CMD_NONE = 0,
    UI_CMD_START_CALIBRATION,
    UI_CMD_START_TEST,
    UI_CMD_START_TEST_K,
    UI_CMD_START_SOFIA,
    UI_CMD_SOFIA_MEASURE,
    UI_CMD_TC_HEAT_BEGIN,
    UI_CMD_TC_HEAT_NEXT,
    UI_CMD_ABORT,
    UI_CMD_TC_BEGIN,
    UI_CMD_TC_CAPTURE,
    UI_CMD_TC_CANCEL,
    UI_CMD_TC_APPLY_FIT,
    UI_CMD_TC_SAVE,
    UI_CMD_TC_RESET,
    UI_CMD_RETURN_HOME
} ui_cmd_type_t;

/* Parâmetros carregados pela tela de configuração do ensaio. */
typedef struct {
    ui_cmd_type_t type;
    sofia_config_t sofia;
    unsigned tc_points;
    float tc_offset, tc_alpha;
    float reference_temperature_c;
    float start_temperature_c;
    float end_temperature_c;
    float step_temperature_c;
    float manual_k_c_per_v;
} ui_cmd_t;

void ui_init(void);
ui_cmd_t ui_step(const device_t *device, input_event_t event);
void ui_render(const device_t *device);
typedef enum {
    UI_SCREEN_SPLASH, UI_SCREEN_HOME, UI_SCREEN_MENU, UI_SCREEN_SETUP,
    UI_SCREEN_CONFIRM_TEST, UI_SCREEN_CONFIRM_CAL,
    UI_SCREEN_STATUS, UI_SCREEN_RUNNING, UI_SCREEN_RESULT, UI_SCREEN_ERROR, UI_SCREEN_TC_CAL, UI_SCREEN_TC_MULTI, UI_SCREEN_TC_FINE, UI_SCREEN_TC_RESET, UI_SCREEN_K_MANUAL, UI_SCREEN_TC_HEAT_SETUP, UI_SCREEN_SOFIA_SETUP, UI_SCREEN_SOFIA_CONFIRM
} ui_screen_t;
ui_screen_t ui_current_screen(void);

#endif
