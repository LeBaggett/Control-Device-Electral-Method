#ifndef ENCODER_H
#define ENCODER_H
#include "input_event.h"

void encoder_init(void);
/* Retorna no máximo um evento pendente por chamada; não bloqueia. */
input_event_t encoder_get_event(void);

#endif
