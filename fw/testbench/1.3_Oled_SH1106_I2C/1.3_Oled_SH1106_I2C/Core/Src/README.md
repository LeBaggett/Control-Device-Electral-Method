# Src - guia de manutenção

Projeto de referência do OLED/SH1106, com arquivos STM32F1. Não substituir diretamente a pinagem/HAL do projeto F407.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [bitmap.h](bitmap.h) | Interface C, tipos e constantes do módulo bitmap. Conferir includes e os consumidores antes de mudar assinaturas. |
| [fonts.c](fonts.c) | Implementação C do módulo fonts. Conferir o header correspondente e as chamadas antes de alterar. |
| [horse_anim.h](horse_anim.h) | Interface C, tipos e constantes do módulo horse_anim. Conferir includes e os consumidores antes de mudar assinaturas. |
| [main.c](main.c) | Entrada da aplicação e inicializações geradas pelo CubeMX; revisar blocos USER CODE antes/depois de regenerar. Entradas/símbolos para consulta: `SystemClock_Config`, `MX_GPIO_Init`, `MX_I2C1_Init`, `main`, `Error_Handler`, `assert_failed`. |
| [norse_new.h](norse_new.h) | Interface C, tipos e constantes do módulo norse_new. Conferir includes e os consumidores antes de mudar assinaturas. |
| [SH1106.c](SH1106.c) | Implementação C do módulo sh1106. Conferir o header correspondente e as chamadas antes de alterar. Entradas/símbolos para consulta: `SH1106_Init`, `SH1106_UpdateScreen`, `SH1106_ToggleInvert`, `SH1106_Fill`, `SH1106_DrawPixel`, `SH1106_GotoXY`, `SH1106_DrawLine`, `SH1106_DrawRectangle`, `SH1106_DrawFilledRectangle`, `SH1106_DrawTriangle`, `SH1106_DrawFilledTriangle`, `SH1106_DrawCircle`. |
| [stm32f1xx_hal_msp.c](stm32f1xx_hal_msp.c) | Biblioteca ST: msp. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `HAL_MspInit`, `HAL_I2C_MspInit`, `HAL_I2C_MspDeInit`. |
| [stm32f1xx_it.c](stm32f1xx_it.c) | Implementação C do módulo stm32f1xx_it. Conferir o header correspondente e as chamadas antes de alterar. Entradas/símbolos para consulta: `NMI_Handler`, `HardFault_Handler`, `MemManage_Handler`, `BusFault_Handler`, `UsageFault_Handler`, `SVC_Handler`, `DebugMon_Handler`, `PendSV_Handler`, `SysTick_Handler`. |
| [syscalls.c](syscalls.c) | Retarget de chamadas de sistema da biblioteca C para o ambiente embarcado. Entradas/símbolos para consulta: `initialise_monitor_handles`, `_getpid`, `_kill`, `_exit`, `_close`, `_fstat`, `_isatty`, `_lseek`, `_open`, `_wait`, `_unlink`, `_times`. |
| [sysmem.c](sysmem.c) | Suporte de memória dinâmica da biblioteca C, respeitando limites de heap/stack do linker. |
| [system_stm32f1xx.c](system_stm32f1xx.c) | Implementação C do módulo system_stm32f1xx. Conferir o header correspondente e as chamadas antes de alterar. Entradas/símbolos para consulta: `SystemInit_ExtMemCtl`, `SystemInit`, `SystemCoreClockUpdate`. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
