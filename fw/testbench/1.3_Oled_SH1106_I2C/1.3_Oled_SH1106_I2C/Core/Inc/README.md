# Inc - guia de manutenção

Projeto de referência do OLED/SH1106, com arquivos STM32F1. Não substituir diretamente a pinagem/HAL do projeto F407.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [fonts.h](fonts.h) | Interface C, tipos e constantes do módulo fonts. Conferir includes e os consumidores antes de mudar assinaturas. |
| [main.h](main.h) | Entrada da aplicação e inicializações geradas pelo CubeMX; revisar blocos USER CODE antes/depois de regenerar. Entradas/símbolos para consulta: `Error_Handler`. |
| [SH1106.h](SH1106.h) | Interface C, tipos e constantes do módulo sh1106. Conferir includes e os consumidores antes de mudar assinaturas. Entradas/símbolos para consulta: `SH1106_Init`, `SH1106_UpdateScreen`, `SH1106_ToggleInvert`, `SH1106_Fill`, `SH1106_DrawPixel`, `SH1106_GotoXY`, `SH1106_DrawLine`, `SH1106_DrawRectangle`, `SH1106_DrawFilledRectangle`, `SH1106_DrawTriangle`, `SH1106_DrawCircle`, `SH1106_DrawFilledCircle`. |
| [stm32f1xx_hal_conf.h](stm32f1xx_hal_conf.h) | Biblioteca ST: conf. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `assert_failed`. |
| [stm32f1xx_it.h](stm32f1xx_it.h) | Interface C, tipos e constantes do módulo stm32f1xx_it. Conferir includes e os consumidores antes de mudar assinaturas. Entradas/símbolos para consulta: `NMI_Handler`, `HardFault_Handler`, `MemManage_Handler`, `BusFault_Handler`, `UsageFault_Handler`, `SVC_Handler`, `DebugMon_Handler`, `PendSV_Handler`, `SysTick_Handler`. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
