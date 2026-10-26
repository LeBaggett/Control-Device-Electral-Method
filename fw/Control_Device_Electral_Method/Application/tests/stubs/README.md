# stubs - guia de manutenção

Testes no computador com hardware simulado/stubs. Usam GCC/Python e verificam contratos; não devem ser incluídos na imagem STM32.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [ff.h](ff.h) | Implementação central do sistema de arquivos FatFs. Preservar versão e configuração compatíveis. |
| [stm32f4xx_hal.h](stm32f4xx_hal.h) | Interface C, tipos e constantes do módulo stm32f4xx_hal. Conferir includes e os consumidores antes de mudar assinaturas. Entradas/símbolos para consulta: `mock_clock`, `HAL_GPIO_WritePin`, `HAL_GPIO_Init`, `HAL_Init`, `HAL_GetTick`, `HAL_RCC_GetPCLK2Freq`, `SystemCoreClockUpdate`, `mock_nop`, `HAL_Delay`, `HAL_ADC_GetValue`, `mock_sd_reset`, `HAL_SD_GetCardState`. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
