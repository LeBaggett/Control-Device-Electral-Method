# IHM Delta DOP-107BV

Integração da IHM no MVP STM32H753. A bancada 0.2.6-hmi-bench implementa escravo Modbus RTU pela UART4 em PC10/TX e PC11/RX, endereço 1, 115200/8N1, recepção DMA circular. Tela inicial somente leitura: três NOVUS, validade, unidade, Ethernet e heartbeat.

- [Integração e limites](INTEGRACAO_UART4.md): firmware, pinos, diagnóstico e validação pendente.
- [Tela de bancada](screens/bench/README.md): fundo 800×480, objetos e mapa completo de registradores.
- [Artes originais](artwork/README.md): referência React/TypeScript fornecida, preservada.
- [Protocolo](protocol/README.md): responsabilidades do canal.

DOPSoft 4.00.16 instalado. Projeto nativo ainda em montagem; os PNG/CSV/JSON são arte e especificação, não um programa já transferido à Delta. A UART4 ainda precisa de validação física. Os ensaios/calibração e comandos operacionais não fazem parte desta primeira tela de bancada.
