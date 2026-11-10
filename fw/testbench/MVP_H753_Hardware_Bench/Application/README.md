# Aplicação da bancada

| Arquivos | Responsabilidade |
|---|---|
| bench_config.h | Versão, rede, enable RS485, baud, paridade, quantidade/endereços e tempos |
| bench_diag.c/.h | Estado geral, Ethernet e JSON da API existente |
| modbus_read.c/.h | CRC16, pedido função 03, validação de resposta, exceção e inteiro assinado |
| rs485_port.c/.h | USART2, DMA circular RX, direção, silêncio de frame e recuperação; g_rs485_debug |
| n1040.c/.h | Consultas alternadas de status/PV, round-robin, validade e JSON; g_n1040_debug |
| board/ | Clock, LEDs e mapa central dos pinos |
| ui/ | Página em Flash e integração HTTPD |
| tests/ | Testes de HTTP e protocolo/estados no computador |

Consulte ../INTEGRACAO_N1040I.md. Nenhum driver de potência é incluído.
