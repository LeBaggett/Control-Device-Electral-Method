# Protocolo da IHM

IHM mestre Modbus RTU, STM32 escravo 1, UART4 PD1/PD0 a 115200/8N1 com conversor automático. Canal independente dos NOVUS. A implementação da bancada é somente leitura FC03/FC04, sem comandos de potência.

Mapa canônico: [holding-registers.csv](../screens/bench/holding-registers.csv). Objetos e convenção de endereços: [tela de bancada](../screens/bench/README.md). Código em Application/hmi_service.c, modbus_slave.c e hmi_uart.c da MVP_H753_Hardware_Bench. Modificações no mapa exigem versionamento e atualização conjunta da tela.
