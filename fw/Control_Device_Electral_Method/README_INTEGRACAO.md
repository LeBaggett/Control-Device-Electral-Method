# Projeto STM32 integrado — v7

Integração do código v6 com CubeMX 6.18.1 / STM32CubeF4 1.28.3.
Encoder atualizado por autorização do usuário: PA15 CH1 / PB3 CH2;
botão PE2. Demais pinos preservados em Application/board/pinout.h.

## Abrir no CubeIDE

Importe esta pasta como projeto existente, usando seu `.project` e
`.cproject`. Os fontes customizados ficam em Application/. Core/, Drivers/,
FATFS/ e Middlewares/ contêm a base gerada. Os testes de Application/tests
estão excluídos da compilação STM32.

O projeto principal será esta pasta, não o esqueleto anterior nem um projeto
de testbench. Feche/desimporte o projeto antigo no CubeIDE sem marcar
exclusão de conteúdo do disco antes de importar o projeto reorganizado.

## Ajustes integrados

- I2C1 PB8/PB9 em 400 kHz no `.ioc` e main gerado.
- main chama board_init/app_init e app_run antes da sequência automática
  gerada, que não é executada novamente. Wrappers em USER CODE 4.
- GPIO seguros antes do clock/periféricos; SELECT medição antes de relé OFF.
- FatFs real habilitado. Inicialização SD feita por board_sd_prepare;
  DISABLE_SD_INIT evita reinicialização pela camada disk I/O.
- As esperas de conclusão SD geradas agora possuem limite de 1 s.
  O próprio HAL/BSP também possui seu timeout: não é um limite global
  de 1 s para todas as operações do cartão.
- Sem pino CD inventado. O BSP gerado mantém IsDetected=SD_PRESENT;
  inicialização/comunicação SDIO e verificação periódica determinam erros.
  Esse retorno não comprova presença física do cartão.
- Sem RTC; `_FS_NORTC=1` usa a data fixa do ffconf. Data/hora FAT não
  representam o relógio real. Tempos experimentais são relativos em CSV.
- Reserva mínima de stack elevada de 1 KiB para 8 KiB; heap 512 B.
  Stack real e tempos de pior caso ainda exigem observação no alvo.
- Não há DMA configurado neste projeto; o wrapper não chama MX_DMA_Init.
- As chamadas para energizar relé ou selecionar heating permanecem recusadas.

## Evidência

Compilação ARM Cortex-M4/FPU hard com arm-none-eabi-gcc 13.3, usando HAL,
CMSIS e FatFs reais desta geração; link do ELF e emissão do binário: PASS.
Este build foi feito pelo compilador instalado no CubeIDE, fora da interface
do IDE. A compilação pela interface CubeIDE ainda deve ser conferida.
Fontes customizados compilados com -Wall -Wextra -Werror.

Tamanho reportado inicialmente: text 50088, data 472, bss 14680 bytes,
incluindo reservas do linker. Build/ contém os artefatos de conferência.
Não foi gravado na placa nem validado fisicamente.

## Regeneração CubeMX

Keep User Code está ativo. Ainda assim, após regenerar, confira os includes
e exclusões do .cproject, `_FS_NORTC`, `DISABLE_SD_INIT`, os dois timeouts de
sd_diskio.c e a reserva de stack. Os timeouts ficam em trechos gerados e
podem ser sobrescritos. Mantenha as customizações USER CODE do main.

Consulte Application/docs/BANCADA.md antes dos testes sem potência.
Application/README.md e VALIDACAO.md descrevem a entrega v6 e seus testes
simulados; esta integração acrescenta o build ARM real, sem validação física.
