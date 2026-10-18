#ifndef BOARD_H
#define BOARD_H
#include <stdint.h>
#include <stdbool.h>
/* board_init: GPIO seguros primeiro, depois clocks/periféricos.
 * A implementação real deve manter relé OFF e SELECT=medição no reset. */
void board_init(void);
uint32_t board_millis(void);
void board_delay_us(uint32_t delay_us);
void board_power_select_measurement(void);
bool board_power_select_heating(void);
void board_power_relay_off(void);
bool board_power_relay_on(void);
bool board_is_ready(void);
bool board_power_control_enabled(void);
void board_force_safe(void);
/* SPI configurado MODE 0, 8 bits, CS ativo baixo; 4 bytes MSB primeiro. */
bool board_max31855_read32(uint32_t *raw);
#endif
