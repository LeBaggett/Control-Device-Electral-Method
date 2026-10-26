#include "power.h"
#include "board.h"
/* Intertravamento lógico. A segurança no reset exige resistores/pinos físicos. */
static bool initialized;
static bool relay_on;
static bool measuring;
void power_safe(void)
{
    /* Ordem obrigatória: selecionar medição ANTES de desligar o relé. */
    board_power_select_measurement();
    board_power_relay_off();
    relay_on = false;
    measuring = true;
}
void power_init(void)
{
    initialized = false;
    power_safe();
    initialized = true;
}
bool power_enable_measurement(void)
{
    if (!initialized) return false;
    board_power_select_measurement();
    measuring = true;
    return true;
}
bool power_relay_on(void)
{
    if (!initialized || !measuring) return false;
    if (!board_power_relay_on()) { power_safe(); return false; }
    relay_on = true;
    return true;
}
bool power_select_heating(void)
{
    if (!initialized || !relay_on) return false;
    if (!board_power_select_heating()) { power_safe(); return false; }
    measuring = false;
    return true;
}
bool power_is_safe(void) { return !relay_on && measuring; }
