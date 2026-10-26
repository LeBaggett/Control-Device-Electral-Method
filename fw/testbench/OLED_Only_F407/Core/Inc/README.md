# Inc - guia de manutenção

Projeto isolado de diagnóstico do OLED no STM32F407. Não executa o ensaio térmico completo.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [main.h](main.h) | Entrada da aplicação e inicializações geradas pelo CubeMX; revisar blocos USER CODE antes/depois de regenerar. Entradas/símbolos para consulta: `Error_Handler`. |
| [oled_probe.h](oled_probe.h) | Interface C, tipos e constantes do módulo oled_probe. Conferir includes e os consumidores antes de mudar assinaturas. Entradas/símbolos para consulta: `oled_test_run`. |
| [pinout.h](pinout.h) | Mapa central dos pinos, polaridades, seleção SSD1306/SH1106 e habilitação da potência. Conferir junto ao .ioc e à montagem. |
| [stm32f4xx_hal_conf.h](stm32f4xx_hal_conf.h) | Seleção e configuração dos módulos HAL usados no projeto. Entradas/símbolos para consulta: `assert_failed`. |
| [stm32f4xx_it.h](stm32f4xx_it.h) | Vetores e handlers das interrupções; manter correspondência com startup e callbacks dos drivers. Entradas/símbolos para consulta: `NMI_Handler`, `HardFault_Handler`, `MemManage_Handler`, `BusFault_Handler`, `UsageFault_Handler`, `SVC_Handler`, `DebugMon_Handler`, `PendSV_Handler`, `SysTick_Handler`. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
