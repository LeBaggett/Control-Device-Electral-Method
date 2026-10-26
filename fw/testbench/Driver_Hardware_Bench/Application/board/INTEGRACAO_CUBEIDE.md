Nesta versão de bancada manual, siga o README.md da raiz: PD12 SELECT, PD14 RELAY, acionamento manual habilitado, sem calibração/ensaio.

# Integração final — STM32F407VET6

Esta etapa pode ser feita após os módulos de fonte. Não conectar potência.
`pinout.h` permanece a fonte central do mapa físico; `config.h` reúne os
parâmetros do método e `runtime_config.h` os perfis de driver.

## `.ioc` e interfaces

| Bloco | Pinos / configuração |
|---|---|
| Relé | PD14, output push-pull LOW, pull-down externo |
| SELECT | PD12, output push-pull HIGH, pull-up externo |
| MAX31855 CS | PA4, output push-pull HIGH, pull-up externo |
| SPI1 | PA5 SCK, PA6 MISO, PA7 MOSI reservado, AF5 |
| Vf | PC0 analog, ADC1_IN10; VREFINT canal interno 17 |
| OLED | PB8 SCL/PB9 SDA, I2C1 AF4 open-drain, 400 kHz, 7 bits 0x3C |
| Encoder | PA0/PA1, TIM2 CH1/CH2 AF1, TI1+TI2 encoder mode |
| Botão | PE2 input ativo baixo, pull-up |
| SDIO | PC8..PC11 D0..D3, PC12 CK, PD2 CMD, AF12, 4 bits |
| Debug | PA13/PA14 SWD reservados |

GPIO de segurança: low speed; escrever os latches SELECT HIGH → PD14 LOW →
CS HIGH antes de configurar saída. O `MX_GPIO_Init()` gerado não pode criar
pulsos opostos. Compare os níveis iniciais do `.ioc` e do código gerado com
pinout.h após cada regeneração. Pulls externos protegem durante reset.

SPI1: master full duplex (`SPI_DIRECTION_2LINES`), 8 bits, MSB, CPOL LOW,
CPHA 1 EDGE, NSS software, sem TI/CRC/DMA. PA7 não vai ao MAX31855.
Com PCLK2=84 MHz, prescaler /32 = 2,625 MHz. O board recusa SCK > 5 MHz.

TIM2: prescaler 0, período **4294967295** (32 bits), entradas não invertidas
inicialmente; filtros ajustados na bancada. O driver requer o período 32 bits.
Quatro contagens por detente e direção são ajustáveis em runtime_config.h.

ADC1: PCLK2 /4 (21 MHz no clock previsto), 12 bits, alinhamento à direita,
scan DISABLE, continuous DISABLE, uma conversão, software start, EOC single
conversion. O adapter muda o rank 1 entre PC0 e VREFINT; não usar DMA ou
outro cliente ADC1. Vf utiliza 15 ciclos; VREFINT 480 ciclos e startup de
10 us. Verificar que a impedância/filtragem em PC0 admite amostragem de
15 ciclos; caso contrário ajustar o driver e medir novamente a janela.
VREFINT_CAL em 0x1FFF7A2A é validado antes de usar; valor/leitura inválidos
usam VDDA_NOMINAL_V=3,3 e a tela mostra `VF EST`. O cálculo de VDDA presume
VREF+ ligado a VDDA, conforme o plano; conferir isso no esquema da placa.

SDIO/FatFs: configurar clocks, MSP, driver de disco, IRQ/DMA de acordo com
a implementação CubeMX escolhida. FatFs read/write e sync habilitados,
volume 0 e nomes 8.3 suficientes. Sem cartão, a interface deve continuar
funcionando. `board_sd_prepare()` substitui a chamada fatal a
`MX_SDIO_SD_Init()` no wrapper; retorna false em cartão ausente e tenta
1 bit → 4 bits. Clock de transferência inicial 12 MHz com SDIOCLK=48 MHz.
Não substituir o teste de erro por Error_Handler. A rotina de disco gerada
também precisa retornar erros/timeout, sem travar em espera infinita.
`MX_FATFS_Init()` vincula o disk driver antes do mount; ajustar os nomes se
o CubeMX gerar outras interfaces. Não usar um stub de disco que retorne
sucesso sem escrever. Depois definir THERMAL_USE_FATFS=1.

Clock: HSE=8 MHz e SYSCLK=168 MHz são o alvo do plano, a confirmar na placa.
Referência se HSE real=8 MHz: PLLM=8/N=336/P=2/Q=7, AHB /1, APB1 /4,
APB2 /2. Confirmar 48 MHz do SDIO, alimentação, Flash wait states e voltage
scaling; Crystal ou Bypass depende da fonte física de HSE.

## Fontes e `main`

1. Gere o projeto CubeIDE a partir do `.ioc` confirmado. Adicione as fontes
   de `target_sources.list` e includes da raiz, board, core, drivers,
   interfaces e ui. Inclua HAL/CMSIS e FatFs gerados. Nunca incluir
   `tests/stubs` no alvo. Não existe mais job_io_unavailable neste pacote:
   os jobs completos chamam drivers e respeitam o bloqueio de potência.
2. Use apenas um main. O `main.c.example` mostra a rotina: board_init,
   app_init e loop chamando app_run. app_init/app_run mantêm estado seguro
   se o board não estiver pronto.
3. Na integração com a main gerada, copie os wrappers de
   `cube_glue.c.example` para USER CODE 4. Eles podem chamar `MX_*` static
   na mesma unidade. Inclua app.h, board.h, sd_io.h, runtime_config.h e,
   quando habilitado, fatfs.h em USER CODE Includes.
4. Coloque o bloco board_init/app_init/superloop em USER CODE BEGIN 1,
   antes de HAL_Init gerado. Ele não retorna à inicialização automática
   abaixo, evitando inicializar duas vezes. Se os periféricos forem gerados
   em `.c/.h` separados, use seus headers públicos nos wrappers.
5. A sequência no wrapper deve continuar DMA → SPI1 → ADC1 → I2C1 → TIM2
   → board_sd_prepare → vínculo FatFs. `MX_DMA_Init` precisa corresponder
   ao DMA realmente configurado. Após regenerar, confira os wrappers e
   os níveis de GPIO, sem mover a segurança para depois dos periféricos.
6. Primeiro ato de Error_Handler: board_force_safe antes de desabilitar IRQ.
   Isso não substitui pulls externos/inibição física em reset/HardFault.

Ordem rigorosa preservada: HAL_Init → GPIO seguro antecipado → clock →
MX_GPIO_Init → reafirmar seguro → DWT → periféricos na ordem acima →
reafirmar seguro → app_init → power_init/safe → sensor/ADC/OLED/encoder/SD
→ carregar calibração → job/UI → power_safe → superloop.

## Antes de qualquer futura liberação de potência

- Conferir continuidade/polaridade PD14 ativo alto e PD12 alto=medição,
  driver do relé, isolamento, corrente e inibição de hardware.
- Medir níveis desde energização/reset/watchdog/debug. Nenhum pulso em
  PD14 HIGH ou PD12 LOW é permitido durante boot desta versão.
- Conferir sensor MAX31855, tipo de termopar, 3,3 V, SPI mode 0 e 32 clocks.
  PA4 fica HIGH fora da leitura. Espera inicial 200 ms, intervalo ≥100 ms.
- Confirmar splash do OLED selecionado e orientação/offset, um evento por detente,
  clique curto/longo sem repique. O bin original é XBM LSB-first por linha.
- Aplicar referência de 0,600 V a PC0 dentro dos limites elétricos e conferir
  ADC/VDDA com instrumentos. Fallback de VDDA não é calibração metrológica.
- Exercitar SD ausente/cheio/removido, arquivo parcial e calibração
  corrompida. Verificar que a interface continua e não há energização.
- Medir no alvo a janela SELECT HIGH → 30 us → 8 amostras; o limite de
  400 us do burst é uma guarda de falha, não prova de janela de dezenas de us.
  DWT e overhead HAL/IRQ precisam ser verificados em Release sem halt.
- Revisar bloqueios de SPI/I2C/FatFs/SD, watchdog e intertravamentos físicos
  antes de remover o bloqueio fixo de potência em outra versão.

Nenhuma dessas verificações físicas foi feita. Neste pacote, chamadas de
relé ON/aquecimento retornam false mesmo se os demais drivers estiverem OK.

Fontes primárias consultadas:
[SH1106 datasheet](https://www.displayfuture.com/Display/datasheet/controller/SH1106.pdf),
[STM32F407 DS8626](https://www.st.com/resource/en/datasheet/stm32f407vg.pdf),
[HAL STM32F4 oficial](https://github.com/STMicroelectronics/stm32f4xx-hal-driver),
[FatFs f_open](https://elm-chan.org/fsw/ff/doc/open.html) e
[FatFs f_sync](https://elm-chan.org/fsw/ff/doc/sync.html).
