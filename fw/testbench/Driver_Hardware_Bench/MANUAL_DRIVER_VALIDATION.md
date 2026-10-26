# Bancada: validação manual de drivers

Firmware exclusivamente para visualizar leituras e comandar hardware manualmente.
Não executa calibração, ensaio térmico, alvos, controle térmico nem decisões de
saída por temperatura, ADC ou estado do SD. O principal não foi alterado.

## IOs

PD14 = relé (HIGH liga); PD12 = SELECT (HIGH medição / LOW aquecimento).
PA1 = LED integrado ativo LOW. Pinos mantidos no pinout.h e .ioc.
A inicialização começa com relé OFF, SELECT medição e LED OFF.
Depois disso, os estados permanecem como comandados. SELECT pode ser comutado
com relé desligado; ligar/desligar relé não muda SELECT. Não há intertravamento
entre esses dois IOs. DESLIGAR TUDO é uma ação manual que põe SELECT em medição
e desliga o relé. Falha em SPI, termopar desconectado, ADC inválido e temperatura
alta não acionam nem desligam saídas. As proteções do principal permanecem lá.
A flag de compilação BOARD_POWER_CONTROL_ENABLED=1 autoriza comandos na bancada.

## Encoder/OLED

Gire para selecionar, clique para executar:

1. RELE LIGAR/DESLIGAR.
2. SELECT AQUECER/MEDICAO.
3. DESLIGAR TUDO (comando explícito).
4. TESTAR SD RW (não muda as saídas).
5. LED ACENDER/APAGAR.
6. VER DADOS (ADC bruto, VDDA, PD14/PD12); clique retorna.

Pressionar LONGO executa a parada manual de relé/SELECT.
O LED não pisca sozinho: é comandado por você.
OLED mostra temperatura bruta MAX31855, ADC em volts e estado dos IOs.
Sem leitura válida, mostra --; isso não bloqueia o controle dos IOs.

## Debugger

Adicione g_bench_control, g_bench_status e g_hw_debug em Expressions.
Para enviar, pause, altere g_bench_control.command, incremente request_id para
valor diferente de ack_id e retome. O pedido é consumido uma vez; ack_id confirma.
Comandos: 1=relé ON; 2=relé OFF; 3=SELECT medição; 4=SELECT aquecimento;
5=desligar relé/SELECT; 6=teste SD; 7=LED ON; 8=LED OFF.
result: 0=sucesso; 1=flag de potência bloqueada; 5=falha SD;
6=comando desconhecido; 7=parada manual pelo botão.
Pausar debug mantém os níveis dos IOs. A bancada não fará desligamento por conta
própria se a carga aquecer ou o termopar for desconectado.

## Drivers disponíveis

- GPIO: PD14/PD12 e LED PA1; encoder PA15/PB3 com botão PE2.
- SPI1/MAX31855: PA4 CS, PA5 SCK, PA6 MISO; PA7 reservado. Temperatura a cada 250 ms.
- ADC1_IN10: PC0, Vf em volts, contagens brutas e VDDA por VREFINT.
- I2C1 OLED PB8/PB9; SSD1306 padrão, SH1106 selecionável em pinout.h.
- SDIO PC8..PC12/PD2: 1 bit/400 kHz, timeout curto e tentativas limitadas de CRC.
- DWT: inicialização/timing do transporte mantidos.

O teste SD cria BCHnnn.TXT novo, grava, sincroniza e relê para comparar. Não
sobrescreve ensaios ou calibrações. Ele não altera relé/SELECT/LED. Pode bloquear
enquanto o driver espera pelo cartão, então a saída fica no estado escolhido.
BENCH_TEST_* em bench_config.h permitem habilitar drivers individualmente.
Não há mais BENCH_REQUIRE_TC_FOR_POWER, limite térmico ou timeout de sensor
com efeito sobre as saídas. Não carrega correções TCAL/CALIB do principal.

## Gravar e testar

Terminate Debug > Refresh > Clean > Build no projeto Driver_Hardware_Bench;
grave esse projeto na placa. O ZIP também inclui binário/ELF compilados.

Testes cobriram IOs independentes, preservação dos níveis sem sensor/ADC, valor
alto de temperatura sem desligamento, falha de SPI/DWT sem mudar IOs, teste SD
sem mudar IOs, LED manual e parada por comando. Drivers/regressões e build ARM
passaram. Não houve validação física nem gravação automática na placa.
