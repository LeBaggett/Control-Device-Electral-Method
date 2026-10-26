# Src - guia de manutenção

Projeto isolado de diagnóstico do OLED no STM32F407. Não executa o ensaio térmico completo.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [main.c](main.c) | Entrada da aplicação e inicializações geradas pelo CubeMX; revisar blocos USER CODE antes/depois de regenerar. Entradas/símbolos para consulta: `safe_gpio`, `clock_hsi`, `main`, `Error_Handler`. |
| [oled_probe.c](oled_probe.c) | Implementação C do módulo oled_probe. Conferir o header correspondente e as chamadas antes de alterar. Entradas/símbolos para consulta: `scl_high`, `sda_high`, `snapshot`, `wait_scl`, `bus_release`, `send_bytes`, `oled_test_run`. |
| [stm32f4xx_hal_msp.c](stm32f4xx_hal_msp.c) | Configuração de clocks, funções alternativas e GPIOs dos periféricos; arquivo gerado com ajustes locais. Entradas/símbolos para consulta: `HAL_MspInit`, `HAL_I2C_MspInit`, `HAL_I2C_MspDeInit`. |
| [stm32f4xx_it.c](stm32f4xx_it.c) | Vetores e handlers das interrupções; manter correspondência com startup e callbacks dos drivers. Entradas/símbolos para consulta: `NMI_Handler`, `HardFault_Handler`, `MemManage_Handler`, `BusFault_Handler`, `UsageFault_Handler`, `SVC_Handler`, `DebugMon_Handler`, `PendSV_Handler`, `SysTick_Handler`. |
| [syscalls.c](syscalls.c) | Retarget de chamadas de sistema da biblioteca C para o ambiente embarcado. Entradas/símbolos para consulta: `initialise_monitor_handles`, `_getpid`, `_kill`, `_exit`, `_close`, `_fstat`, `_isatty`, `_lseek`, `_open`, `_wait`, `_unlink`, `_times`. |
| [sysmem.c](sysmem.c) | Suporte de memória dinâmica da biblioteca C, respeitando limites de heap/stack do linker. |
| [system_stm32f4xx.c](system_stm32f4xx.c) | Configuração básica de sistema/clock do núcleo fornecida com CMSIS/ST. Entradas/símbolos para consulta: `SystemInit_ExtMemCtl`, `SystemInit`, `SystemCoreClockUpdate`. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
