#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "power.h"
#include "max31855.h"
static char sequence[32]; static int n;
static void record(char c) { sequence[n++] = c; sequence[n] = 0; }
void board_power_select_measurement(void) { record('M'); }
bool board_power_select_heating(void) { record('H'); return true; }
void board_power_relay_off(void) { record('O'); }
bool board_power_relay_on(void) { record('R'); return true; }
bool board_max31855_read32(uint32_t *raw) { *raw = (100U << 18); return true; }
int main(void)
{
    float t, internal;
    power_init(); assert(power_is_safe()); assert(sequence[0]=='M' && sequence[1]=='O');
    assert(!power_select_heating());
    assert(power_enable_measurement()); assert(power_relay_on());
    assert(power_select_heating()); assert(!power_is_safe());
    n=0; sequence[0]=0; power_safe(); assert(sequence[0]=='M' && sequence[1]=='O');
    assert(power_is_safe());
    assert(max31855_read(&t)==MAX31855_OK && fabsf(t-25.0f)<0.001f);
    assert(max31855_decode(1U<<16 | 1U, &t, &internal)==MAX31855_OPEN_CIRCUIT);
    assert(max31855_decode(1U<<16 | 2U, &t, &internal)==MAX31855_SHORT_GND);
    assert(max31855_decode(1U<<16 | 4U, &t, &internal)==MAX31855_SHORT_VCC);
    assert(max31855_decode(1U<<17, &t, &internal)==MAX31855_FRAME_ERROR);
    assert(max31855_decode((0x3FFCU<<18), &t, &internal)==MAX31855_OK);
    assert(fabsf(t+1.0f)<0.001f);
    puts("power + MAX31855 tests: PASS"); return 0;
}

uint32_t board_millis(void) { return 0; }
