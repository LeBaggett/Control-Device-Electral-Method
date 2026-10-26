# Driver_Hardware_Bench - guia de manutenção

Projeto manual de validação de drivers e comunicação. O estado local inclui diagnóstico SD somente de leitura e pode divergir da versão oficial entregue.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Uso atual

Controle manual de relé/SELECT e visualização de termopar/ADC/OLED/encoder. Não é o projeto oficial entregue.

A opção atual de SD é `SD DIAG LEITURA`, sem criação do arquivo de roundtrip: seis leituras dos setores 0 e 32768 com diagnóstico retido em `g_sd_read_diag`. O setor 32768 foi escolhido para o cartão investigado; não presumir que toda partição começa nesse LBA.

O driver local espera TRANSFER com prazo limitado e repete setor 0 totalmente zerado até o limite. `g_sd_io_debug.read_zero_sector_retries` registra a recuperação. Dados de setores comuns podem ser todos zero e não devem ser tratados como cabeçalho inválido.

Diagnóstico somente leitura não valida escrita. O firmware entregue possui seu próprio caminho de recuperação; comparar arquivos antes de transportar alterações entre projetos.


## Subpastas

| Pasta | Responsabilidade |
|---|---|
| [Application](Application/README.md) | Projeto manual de validação de drivers e comunicação. O estado local inclui diagnóstico SD somente de leitura e pode divergir da versão oficial entregue. |
| [Core](Core/README.md) | Projeto manual de validação de drivers e comunicação. O estado local inclui diagnóstico SD somente de leitura e pode divergir da versão oficial entregue. |
| [Drivers](Drivers/README.md) | Bibliotecas de periféricos do fabricante. Manter fontes e headers da mesma versão; registrar qualquer ajuste local antes de atualizar o pacote. |
| [FATFS](FATFS/README.md) | Projeto manual de validação de drivers e comunicação. O estado local inclui diagnóstico SD somente de leitura e pode divergir da versão oficial entregue. |
| [Middlewares](Middlewares/README.md) | Middleware FatFs e suporte: implementação do sistema de arquivos. Configuração de placa está em FATFS/Target, não neste diretório. |

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [.cproject](.cproject) | Metadados Eclipse/CubeIDE: identidade do projeto, toolchain, includes e exclusões de compilação. |
| [.gitignore](.gitignore) | Arquivo de suporte .gitignore; identificar seu consumidor antes de modificar ou remover. |
| [.mxproject](.mxproject) | Arquivo de suporte .mxproject; identificar seu consumidor antes de modificar ou remover. |
| [.project](.project) | Metadados Eclipse/CubeIDE: identidade do projeto, toolchain, includes e exclusões de compilação. |
| [BOOT_SD_DIAGNOSTICS.md](BOOT_SD_DIAGNOSTICS.md) | Documento específico: boot sd diagnostics. Conferir a data/contexto e confrontar afirmações históricas com o código entregue. |
| [Driver_Hardware_Bench.ioc](Driver_Hardware_Bench.ioc) | Projeto CubeMX: pinos, clocks, periféricos e gerador. Regenerar pode sobrescrever código fora de USER CODE. |
| [Driver_Hardware_Bench.launch](Driver_Hardware_Bench.launch) | Configuração de execução/depuração do CubeIDE. Conferir ELF, ST-LINK e portas locais antes de reutilizar. |
| [EXFAT_SUPPORT.md](EXFAT_SUPPORT.md) | Documento específico: exfat support. Conferir a data/contexto e confrontar afirmações históricas com o código entregue. |
| [LIVE_DEBUG.md](LIVE_DEBUG.md) | Documento específico: live debug. Conferir a data/contexto e confrontar afirmações históricas com o código entregue. |
| [MANUAL_DRIVER_VALIDATION.md](MANUAL_DRIVER_VALIDATION.md) | Documento específico: manual driver validation. Conferir a data/contexto e confrontar afirmações históricas com o código entregue. |
| [OLED_SELECTION.md](OLED_SELECTION.md) | Documento específico: oled selection. Conferir a data/contexto e confrontar afirmações históricas com o código entregue. |
| [POWER_PINOUT_UPDATE.md](POWER_PINOUT_UPDATE.md) | Documento específico: power pinout update. Conferir a data/contexto e confrontar afirmações históricas com o código entregue. |
| [SD_COMPLETE_READ_FIX.md](SD_COMPLETE_READ_FIX.md) | Documento específico: sd complete read fix. Conferir a data/contexto e confrontar afirmações históricas com o código entregue. |
| [SD_MOUNT_TRACE.md](SD_MOUNT_TRACE.md) | Documento específico: sd mount trace. Conferir a data/contexto e confrontar afirmações históricas com o código entregue. |
| [STM32F407VETX_FLASH.ld](STM32F407VETX_FLASH.ld) | Script de ligação: regiões Flash/RAM, seções, heap e stack. Deve corresponder ao MCU e ao startup. |
| [STM32F407VETX_RAM.ld](STM32F407VETX_RAM.ld) | Script de ligação: regiões Flash/RAM, seções, heap e stack. Deve corresponder ao MCU e ao startup. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)


## Documentação anterior preservada

O conteúdo abaixo registra etapas anteriores. Em caso de divergência, conferir o guia atual acima e os fontes da versão correspondente.

<details>
<summary>Abrir documentação anterior</summary>

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

## Partida do OLED independente

O boot inicializa I2C/OLED e mostra o painel antes dos outros drivers da aplicação.
Não inicializa nem monta cartão automaticamente. SD começa como não testado;
selecione TESTAR SD RW (ou comando 6) para iniciar/montar/escrever/reler.
DWT indisponível é reportado, mas não impede a inicialização do OLED.
Refresh, Clean e Build no projeto Driver_Hardware_Bench e grave esse projeto.
As saídas continuam estritamente manuais, sem decisões baseadas em leituras.
Se a tela continuar apagada, pause e confira g_bench_status.loop_count,
g_bench_status.oled, g_hw_debug.oled_prepare_attempts e oled_last_hal_status.

</details>
