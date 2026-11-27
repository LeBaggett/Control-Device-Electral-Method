# Aplicação da bancada

| Arquivos | Responsabilidade |
|---|---|
| bench_config.h | Versão, rede, enable RS485, baud, paridade, quantidade/endereços e tempos |
| bench_diag.c/.h | Estado geral, Ethernet e JSON da API existente |
| modbus_read.c/.h | CRC16, pedido função 03, validação de resposta, exceção e inteiro assinado |
| rs485_port.c/.h | USART2, DMA circular RX, direção, silêncio de frame e recuperação; g_rs485_debug |
| modbus_slave.c/.h | Servidor de leitura FC03/04, limites, CRC, exceções e rejeição de escritas |
| hmi_uart.c/.h | UART4 PD1/PD0, DMA1 Stream1 e eventos encaminhados pelos callbacks HAL compartilhados |
| hmi_service.c/.h | Mapa de 64 palavras, snapshots e diagnóstico da IHM; g_hmi_debug |
| n1040.c/.h | Consultas alternadas de status/PV, round-robin, validade e JSON; g_n1040_debug |
| board/ | Clock, LEDs e mapa central dos pinos |
| ui/ | Página em Flash e integração HTTPD |
| tests/ | Testes de HTTP e protocolo/estados no computador |

Consulte ../INTEGRACAO_N1040I.md. Nenhum driver de potência é incluído.

## ADC + DMA

`adc_service.c/.h` integra ADC1, TIM6 e DMA circular; `adc_stats.c/.h` calcula estat�sticas e tens�o nominal por bloco. Detalhes, limites e mapa da IHM em [README_ADC.md](README_ADC.md).
