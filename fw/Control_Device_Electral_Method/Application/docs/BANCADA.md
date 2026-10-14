# Conferência pendente — manter potência desconectada

| Etapa | Resultado esperado | Situação |
|---|---|---|
| Compilar CubeIDE/HAL/FatFs reais | Sem símbolos ausentes; fontes/tests separados; memória/stack conferidas | Pendente |
| Boot/reset/watchdog | PE1 HIGH, PE0 LOW, PA4 HIGH sem pulso oposto | Pendente |
| Splash | Instituto CAIS conforme `preview/row_lsb.png`, sem texto espelhado | Pendente |
| OLED e menus | SH1106 128x64, coluna correta; navegação/edição/clique/long press | Pendente |
| MAX31855 | Temperatura plausível comparada com referência; aberto gera falha | Pendente |
| ADC | PC0 a 0,600 V lido corretamente; VDDA conferido com multímetro | Pendente |
| Encoder | Um evento por detente e sentido correto, sem repique | Pendente |
| SD sem cartão | Interface continua, início recusado sem relé ON | Pendente |
| SD cheio/removido/curto write | Retorna erro e encerra job seguro, sem espera infinita | Pendente |
| Arquivos | Numeração sem sobrescrever; sync; reload e CRC/backup da calibração | Pendente |
| DWT/ADC | Medir 30 us + captura em Release e tempos de pior caso com IRQ | Pendente |
| Potência | Toda tentativa de ON/heating recusada nesta versão | Pendente na placa; PASS em mock |

Os testes de calibração e autoaquecimento descritos no documento original
não devem ser executados em uma carga nesta versão. O código de board
continua recusando potência; uma etapa posterior precisa revisar
intertravamentos, corrente, limites e tempos e autorizar a liberação.

Registrar firmware/hash, configuração `.ioc`, instrumentos, capturas,
cartão usado, temperatura e resultado. Não preencher PASS sem executar.
