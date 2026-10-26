#ifndef INPUT_EVENT_H
#define INPUT_EVENT_H

/* Eventos lógicos independentes do hardware do encoder. */
typedef enum {
    INPUT_EVENT_NONE = 0,
    INPUT_EVENT_CW,
    INPUT_EVENT_CCW,
    INPUT_EVENT_CLICK,
    INPUT_EVENT_LONG
} input_event_t;

#endif
