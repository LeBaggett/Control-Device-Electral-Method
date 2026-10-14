#ifndef UI_H
#define UI_H
#include "device.h"
#include "input_event.h"

typedef enum {
    UI_CMD_NONE = 0,
    UI_CMD_START_CALIBRATION,
    UI_CMD_START_TEST,
    UI_CMD_ABORT,
    UI_CMD_RETURN_HOME
} ui_cmd_type_t;

/* Parâmetros carregados pela tela de configuração do ensaio. */
typedef struct {
    ui_cmd_type_t type;
    float start_temperature_c;
    float end_temperature_c;
    float step_temperature_c;
} ui_cmd_t;

void ui_init(void);
ui_cmd_t ui_step(const device_t *device, input_event_t event);
void ui_render(const device_t *device);
typedef enum {
    UI_SCREEN_SPLASH, UI_SCREEN_HOME, UI_SCREEN_MENU, UI_SCREEN_SETUP,
    UI_SCREEN_CONFIRM_TEST, UI_SCREEN_CONFIRM_CAL,
    UI_SCREEN_STATUS, UI_SCREEN_RUNNING, UI_SCREEN_RESULT, UI_SCREEN_ERROR
} ui_screen_t;
ui_screen_t ui_current_screen(void);

#endif
