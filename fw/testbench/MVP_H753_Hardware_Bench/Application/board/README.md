# Adaptação da placa

- `pinout.h`: mapa central dos LEDs, RMII e USART2/RS485 (PD5 TX, PD6 RX, PD4 DE+/RE juntos).
- `board.c`: inicializa LEDs, alimentação/clock HSI+PLL e SRAM D2.
- `board.h`: contrato da inicialização e LEDs.

A configuração UART/DMA fica em `../rs485_port.c`, que utiliza os pinos deste header. Ao trocar de UART/portas, revisar também AF, clocks, request DMA, IRQ e startup; apenas trocar um número de pino não remapeia o periférico.
