#ifndef POWER_H
#define POWER_H
#include <stdbool.h>
/* SELECT alto = medição; baixo = aquecimento (CONFERIR no esquema). */
void power_init(void);
void power_safe(void);
bool power_enable_measurement(void);
bool power_select_heating(void);
/* Habilitação permitida somente após SELECT em medição. */
bool power_relay_on(void);
bool power_is_safe(void);
#endif
