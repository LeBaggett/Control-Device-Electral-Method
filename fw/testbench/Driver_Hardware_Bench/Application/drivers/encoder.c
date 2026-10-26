#include "encoder.h"
#include "hw_debug.h"
#include "peripheral_io.h"
#include "board.h"
#include "runtime_config.h"
#include <stdbool.h>
#include <stdint.h>
_Static_assert(THERMAL_ENCODER_COUNTS_PER_DETENT > 0, "Invalid encoder detent");
_Static_assert(THERMAL_ENCODER_DIRECTION == 1 || THERMAL_ENCODER_DIRECTION == -1,
               "Encoder direction must be +/-1");
static bool ready, raw_button, stable_button, armed, long_sent;
static uint32_t last_counter, edge_ms, pressed_ms;
static int32_t remainder;
void encoder_init(void)
{
    ready = board_encoder_start(&last_counter);
    remainder = 0;
    raw_button = stable_button = board_encoder_button_pressed();
    armed = !stable_button; long_sent = false;
    edge_ms = pressed_ms = board_millis();
}
static input_event_t encoder_poll(void)
{
    if (!ready) return INPUT_EVENT_NONE;
    uint32_t now = board_millis(), counter = board_encoder_counter();
    uint32_t difference = counter - last_counter;
    int64_t delta = difference <= INT32_MAX ? (int64_t)difference :
                    -(int64_t)(UINT32_MAX-difference+1U);
    last_counter = counter;
    g_hw_debug.encoder_last_counter = counter;
    g_hw_debug.encoder_delta = (int32_t)delta;
    if (delta < -128 || delta > 128) { remainder = 0; g_hw_debug.encoder_rejected_jumps++; } /* Noise/long polling gap. */
    else {
        int direction = g_hw_tuning.encoder_direction == -1 ? -1 : 1;
        remainder += (int32_t)delta * direction;
        if (remainder > 128) remainder = 128;
        if (remainder < -128) remainder = -128;
    }
    bool pressed = board_encoder_button_pressed();
    if (pressed != raw_button) { raw_button = pressed; edge_ms = now; }
    if (raw_button != stable_button &&
        (uint32_t)(now-edge_ms) >= (g_hw_tuning.button_debounce_ms <= 100U ? g_hw_tuning.button_debounce_ms : 15U)) {
        stable_button = raw_button;
        if (stable_button) { pressed_ms = now; long_sent = false; }
        else {
            bool click = armed && !long_sent;
            armed = true;
            if (click) return INPUT_EVENT_CLICK;
        }
    }
    if (armed && stable_button && !long_sent &&
        (uint32_t)(now-pressed_ms) >= THERMAL_BUTTON_LONG_MS) {
        long_sent = true; return INPUT_EVENT_LONG;
    }
    int32_t counts = (int32_t)g_hw_tuning.encoder_counts_per_detent;
    if (counts < 1 || counts > 8) counts = THERMAL_ENCODER_COUNTS_PER_DETENT;
    if (remainder >= counts) {
        remainder -= counts; return INPUT_EVENT_CW;
    }
    if (remainder <= -counts) {
        remainder += counts; return INPUT_EVENT_CCW;
    }
    return INPUT_EVENT_NONE;
}

input_event_t encoder_get_event(void)
{
 input_event_t event = encoder_poll();
 g_hw_debug.encoder_remainder = remainder;
 g_hw_debug.encoder_event = event;
 if (event == INPUT_EVENT_CW) g_hw_debug.encoder_cw++;
 if (event == INPUT_EVENT_CCW) g_hw_debug.encoder_ccw++;
 if (event == INPUT_EVENT_CLICK) g_hw_debug.encoder_clicks++;
 if (event == INPUT_EVENT_LONG) g_hw_debug.encoder_long++;
 return event;
}
