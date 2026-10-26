#ifndef APP_H
#define APP_H
#include "device.h"

/* Inicializa a aplicação e seus módulos, depois executa um passo cooperativo. */
void app_init(void);
void app_run(void);
const device_t *app_device(void);

#endif
