# Control_Device_Electral_Method - guia de manutenção

Pasta integrante do firmware oficial entregue; use o inventário e os links abaixo para localizar interfaces, implementações e arquivos de suporte.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Versão de referência e operação

Base entregue: commit `aff198f`, importado do ZIP estável informado pelo responsável em 26/10/2026. Esta documentação não modifica o protocolo. `20dfbf5` no nome do ZIP é uma identificação da origem; não significa que o HEAD local tenha esse hash.

- Entrada: `Core/Src/main.c`; inicialização uma vez e superloop em `Application/app.c`.
- Potência: PD14 relé ativo alto; PD12 SELECT alto = medição, baixo = aquecimento, com pull-down. Boot mantém relé desligado.
- Correntes: medição nominal 2 mA ajustável fisicamente por trimpot interno; aquecimento declarado 758 mA. Não são reguladas pelo menu.
- Sensores: MAX31855 na caixa do DUT; Vf em PC0. OLED I2C PB8/PB9; encoder PA15/PB3 e botão PE2.
- SD: SDIO em 1 bit, aproximadamente 400 kHz; FAT32/exFAT. Preservar a implementação entregue, inclusive ajustes HAL/BSP, callbacks e remount limitado no boot.

## Sequência de manutenção

1. Salvar versão/backup, identificar a versão gravada e obter os valores de diagnóstico antes de reiniciar.
2. Alterar pinagem em `Application/board/pinout.h` e conferir `.ioc`, GPIO/MSP e circuito em conjunto.
3. Regenerar com atenção a USER CODE; comparar especialmente SDIO/HAL/FATFS e a ordem de inicialização.
4. Compilar para STM32F407VET6, conferir includes/linker e gravar o ELF da compilação atual.
5. Verificar na bancada: boot seguro, OLED, encoder, termopar, ADC, montagem/leitura/escrita do SD e só então os ensaios.

Os testes locais em `Application/tests` são auxiliares de desenvolvimento e podem pertencer a etapas anteriores; uma falha deve ser investigada sem reescrever a versão entregue para satisfazer uma expectativa antiga.


## Subpastas

| Pasta | Responsabilidade |
|---|---|
| [Application](Application/README.md) | Código próprio do firmware oficial. app coordena board, drivers, core e UI; configuração central em config.h e board/runtime_config.h. |
| [Core](Core/README.md) | Entrada, inicializações geradas, handlers e startup do STM32. Preservar USER CODE e revisar o diff após regenerar o .ioc. |
| [Drivers](Drivers/README.md) | Bibliotecas de periféricos do fabricante. Manter fontes e headers da mesma versão; registrar qualquer ajuste local antes de atualizar o pacote. |
| [FATFS](FATFS/README.md) | Integração CubeMX/FatFs: registro do volume, configuração do filesystem e ponte de operações para BSP/HAL SD. |
| [firmware](firmware/README.md) | Estrutura auxiliar/legada de testes. O projeto ativo é Core + Application + FATFS + Drivers + Middlewares; conferir .cproject antes de adicionar fontes desta pasta ao build. |
| [Middlewares](Middlewares/README.md) | Middleware FatFs e suporte: implementação do sistema de arquivos. Configuração de placa está em FATFS/Target, não neste diretório. |

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [.cproject](.cproject) | Metadados Eclipse/CubeIDE: identidade do projeto, toolchain, includes e exclusões de compilação. |
| [.gitignore](.gitignore) | Arquivo de suporte .gitignore; identificar seu consumidor antes de modificar ou remover. |
| [.mxproject](.mxproject) | Arquivo de suporte .mxproject; identificar seu consumidor antes de modificar ou remover. |
| [.project](.project) | Metadados Eclipse/CubeIDE: identidade do projeto, toolchain, includes e exclusões de compilação. |
| [BOOT_SD_DIAGNOSTICS.md](BOOT_SD_DIAGNOSTICS.md) | Documento específico: boot sd diagnostics. Conferir a data/contexto e confrontar afirmações históricas com o código entregue. |
| [Control-Device-Electral-Method.ioc](Control-Device-Electral-Method.ioc) | Projeto CubeMX: pinos, clocks, periféricos e gerador. Regenerar pode sobrescrever código fora de USER CODE. |
| [Control-Device-Electral-Method.launch](Control-Device-Electral-Method.launch) | Configuração de execução/depuração do CubeIDE. Conferir ELF, ST-LINK e portas locais antes de reutilizar. |
| [Control-Device-Electral-Method_Bancada.launch](Control-Device-Electral-Method_Bancada.launch) | Configuração de execução/depuração do CubeIDE. Conferir ELF, ST-LINK e portas locais antes de reutilizar. |
| [EXFAT_SUPPORT.md](EXFAT_SUPPORT.md) | Documento específico: exfat support. Conferir a data/contexto e confrontar afirmações históricas com o código entregue. |
| [IMMEDIATE_VF_CAPTURE.md](IMMEDIATE_VF_CAPTURE.md) | Documento específico: immediate vf capture. Conferir a data/contexto e confrontar afirmações históricas com o código entregue. |
| [INTEGRACAO_K_TERMOPAR.md](INTEGRACAO_K_TERMOPAR.md) | Documento específico: integracao k termopar. Conferir a data/contexto e confrontar afirmações históricas com o código entregue. |
| [LICENSE](LICENSE) | Licença aplicável aos arquivos distribuídos nesta pasta. Preservar atribuições e condições. |
| [LIVE_DEBUG.md](LIVE_DEBUG.md) | Documento específico: live debug. Conferir a data/contexto e confrontar afirmações históricas com o código entregue. |
| [OLED_SELECTION.md](OLED_SELECTION.md) | Documento específico: oled selection. Conferir a data/contexto e confrontar afirmações históricas com o código entregue. |
| [POWER_PINOUT_UPDATE.md](POWER_PINOUT_UPDATE.md) | Documento específico: power pinout update. Conferir a data/contexto e confrontar afirmações históricas com o código entregue. |
| [README_INTEGRACAO.md](README_INTEGRACAO.md) | Documento específico: readme integracao. Conferir a data/contexto e confrontar afirmações históricas com o código entregue. |
| [SD_CRC_DIAGNOSTICS.md](SD_CRC_DIAGNOSTICS.md) | Documento específico: sd crc diagnostics. Conferir a data/contexto e confrontar afirmações históricas com o código entregue. |
| [SD_READ_DIAGNOSTICS.md](SD_READ_DIAGNOSTICS.md) | Documento específico: sd read diagnostics. Conferir a data/contexto e confrontar afirmações históricas com o código entregue. |
| [SOFIA_ENSAIO.md](SOFIA_ENSAIO.md) | Documento específico: sofia ensaio. Conferir a data/contexto e confrontar afirmações históricas com o código entregue. |
| [STM32F407VETX_FLASH.ld](STM32F407VETX_FLASH.ld) | Script de ligação: regiões Flash/RAM, seções, heap e stack. Deve corresponder ao MCU e ao startup. |
| [STM32F407VETX_RAM.ld](STM32F407VETX_RAM.ld) | Script de ligação: regiões Flash/RAM, seções, heap e stack. Deve corresponder ao MCU e ao startup. |
| [TC_FULL_RESET.md](TC_FULL_RESET.md) | Documento específico: tc full reset. Conferir a data/contexto e confrontar afirmações históricas com o código entregue. |
| [THERMOCOUPLE_MENU.md](THERMOCOUPLE_MENU.md) | Documento específico: thermocouple menu. Conferir a data/contexto e confrontar afirmações históricas com o código entregue. |
| [TSP_ENSAIO.md](TSP_ENSAIO.md) | Documento específico: tsp ensaio. Conferir a data/contexto e confrontar afirmações históricas com o código entregue. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
