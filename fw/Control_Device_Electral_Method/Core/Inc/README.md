# Inc - guia de manutenção

Entrada, inicializações geradas, handlers e startup do STM32. Preservar USER CODE e revisar o diff após regenerar o .ioc.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [main.h](main.h) | Entrada da aplicação e inicializações geradas pelo CubeMX; revisar blocos USER CODE antes/depois de regenerar. Entradas/símbolos para consulta: `Error_Handler`. |
| [stm32f4xx_hal_conf.h](stm32f4xx_hal_conf.h) | Seleção e configuração dos módulos HAL usados no projeto. Entradas/símbolos para consulta: `assert_failed`. |
| [stm32f4xx_it.h](stm32f4xx_it.h) | Vetores e handlers das interrupções; manter correspondência com startup e callbacks dos drivers. Entradas/símbolos para consulta: `NMI_Handler`, `HardFault_Handler`, `MemManage_Handler`, `BusFault_Handler`, `UsageFault_Handler`, `SVC_Handler`, `DebugMon_Handler`, `PendSV_Handler`, `SysTick_Handler`. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
