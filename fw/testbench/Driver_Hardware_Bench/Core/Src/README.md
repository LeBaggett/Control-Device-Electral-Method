# Src - guia de manutenção

Projeto manual de validação de drivers e comunicação. O estado local inclui diagnóstico SD somente de leitura e pode divergir da versão oficial entregue.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [main.c](main.c) | Entrada da aplicação e inicializações geradas pelo CubeMX; revisar blocos USER CODE antes/depois de regenerar. Entradas/símbolos para consulta: `SystemClock_Config`, `MX_GPIO_Init`, `MX_ADC1_Init`, `MX_SDIO_SD_Init`, `MX_SPI1_Init`, `MX_TIM2_Init`, `MX_I2C1_Init`, `main`, `board_cube_clock_init`, `board_cube_gpio_init`, `board_cube_peripherals_init`, `Error_Handler`. |
| [stm32f4xx_hal_msp.c](stm32f4xx_hal_msp.c) | Configuração de clocks, funções alternativas e GPIOs dos periféricos; arquivo gerado com ajustes locais. Entradas/símbolos para consulta: `HAL_MspInit`, `HAL_ADC_MspInit`, `HAL_ADC_MspDeInit`, `HAL_I2C_MspInit`, `HAL_I2C_MspDeInit`, `HAL_SD_MspInit`, `HAL_SD_MspDeInit`, `HAL_SPI_MspInit`, `HAL_SPI_MspDeInit`, `HAL_TIM_Encoder_MspInit`, `HAL_TIM_Encoder_MspDeInit`. |
| [stm32f4xx_it.c](stm32f4xx_it.c) | Vetores e handlers das interrupções; manter correspondência com startup e callbacks dos drivers. Entradas/símbolos para consulta: `NMI_Handler`, `HardFault_Handler`, `MemManage_Handler`, `BusFault_Handler`, `UsageFault_Handler`, `SVC_Handler`, `DebugMon_Handler`, `PendSV_Handler`, `SysTick_Handler`. |
| [syscalls.c](syscalls.c) | Retarget de chamadas de sistema da biblioteca C para o ambiente embarcado. Entradas/símbolos para consulta: `initialise_monitor_handles`, `_getpid`, `_kill`, `_exit`, `_close`, `_fstat`, `_isatty`, `_lseek`, `_open`, `_wait`, `_unlink`, `_times`. |
| [sysmem.c](sysmem.c) | Suporte de memória dinâmica da biblioteca C, respeitando limites de heap/stack do linker. |
| [system_stm32f4xx.c](system_stm32f4xx.c) | Configuração básica de sistema/clock do núcleo fornecida com CMSIS/ST. Entradas/símbolos para consulta: `SystemInit_ExtMemCtl`, `SystemInit`, `SystemCoreClockUpdate`. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
