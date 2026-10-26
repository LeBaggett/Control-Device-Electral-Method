# Src - guia de manutenção

Bibliotecas de periféricos do fabricante. Manter fontes e headers da mesma versão; registrar qualquer ajuste local antes de atualizar o pacote.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [stm32f1xx_hal.c](stm32f1xx_hal.c) | Implementação C do módulo stm32f1xx_hal. Conferir o header correspondente e as chamadas antes de alterar. Entradas/símbolos para consulta: `HAL_MspInit`, `HAL_MspDeInit`, `HAL_IncTick`, `HAL_GetTick`, `HAL_GetTickPrio`, `HAL_Delay`, `HAL_SuspendTick`, `HAL_ResumeTick`, `HAL_GetHalVersion`, `HAL_GetREVID`, `HAL_GetDEVID`, `HAL_GetUIDw0`. |
| [stm32f1xx_hal_cortex.c](stm32f1xx_hal_cortex.c) | Biblioteca ST: interrupções e recursos do núcleo. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `HAL_NVIC_SetPriorityGrouping`, `HAL_NVIC_SetPriority`, `HAL_NVIC_EnableIRQ`, `HAL_NVIC_DisableIRQ`, `HAL_NVIC_SystemReset`, `HAL_SYSTICK_Config`, `HAL_MPU_Disable`, `HAL_MPU_Enable`, `HAL_MPU_ConfigRegion`, `HAL_NVIC_GetPriorityGrouping`, `HAL_NVIC_GetPriority`, `HAL_NVIC_SetPendingIRQ`. |
| [stm32f1xx_hal_dma.c](stm32f1xx_hal_dma.c) | Biblioteca ST: transferências DMA. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `DMA_SetConfig`, `HAL_DMA_IRQHandler`, `HAL_DMA_GetError`. |
| [stm32f1xx_hal_exti.c](stm32f1xx_hal_exti.c) | Biblioteca ST: exti. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `HAL_EXTI_IRQHandler`, `HAL_EXTI_GetPending`, `HAL_EXTI_ClearPending`, `HAL_EXTI_GenerateSWI`. |
| [stm32f1xx_hal_flash.c](stm32f1xx_hal_flash.c) | Biblioteca ST: memória Flash. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `FLASH_Program_HalfWord`, `FLASH_SetErrorCode`, `HAL_FLASH_IRQHandler`, `HAL_FLASH_EndOfOperationCallback`, `HAL_FLASH_OperationErrorCallback`, `HAL_FLASH_OB_Launch`, `HAL_FLASH_GetError`. |
| [stm32f1xx_hal_flash_ex.c](stm32f1xx_hal_flash_ex.c) | Biblioteca ST: operações adicionais de Flash. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `FLASH_MassErase`, `FLASH_PageErase`, `FLASH_OB_GetWRP`, `FLASH_OB_GetRDP`, `FLASH_OB_GetUser`, `HAL_FLASHEx_OBGetConfig`, `HAL_FLASHEx_OBGetUserData`. |
| [stm32f1xx_hal_gpio.c](stm32f1xx_hal_gpio.c) | Biblioteca ST: GPIOs e funções alternativas. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `HAL_GPIO_Init`, `HAL_GPIO_DeInit`, `HAL_GPIO_WritePin`, `HAL_GPIO_TogglePin`, `HAL_GPIO_EXTI_IRQHandler`, `HAL_GPIO_EXTI_Callback`. |
| [stm32f1xx_hal_gpio_ex.c](stm32f1xx_hal_gpio_ex.c) | Biblioteca ST: gpio ex. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `HAL_GPIOEx_ConfigEventout`, `HAL_GPIOEx_EnableEventout`, `HAL_GPIOEx_DisableEventout`. |
| [stm32f1xx_hal_i2c.c](stm32f1xx_hal_i2c.c) | Biblioteca ST: comunicação I2C do OLED. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `I2C_DMAXferCplt`, `I2C_DMAError`, `I2C_DMAAbort`, `I2C_ITError`, `I2C_MasterTransmit_TXE`, `I2C_MasterTransmit_BTF`, `I2C_MasterReceive_RXNE`, `I2C_MasterReceive_BTF`, `I2C_Master_SB`, `I2C_Master_ADD10`, `I2C_Master_ADDR`, `I2C_SlaveTransmit_TXE`. |
| [stm32f1xx_hal_pwr.c](stm32f1xx_hal_pwr.c) | Biblioteca ST: controle de energia do MCU. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `PWR_OverloadWfe`, `HAL_PWR_DeInit`, `HAL_PWR_EnableBkUpAccess`, `HAL_PWR_DisableBkUpAccess`, `HAL_PWR_ConfigPVD`, `HAL_PWR_EnablePVD`, `HAL_PWR_DisablePVD`, `HAL_PWR_EnableWakeUpPin`, `HAL_PWR_DisableWakeUpPin`, `HAL_PWR_EnterSLEEPMode`, `HAL_PWR_EnterSTOPMode`, `HAL_PWR_EnterSTANDBYMode`. |
| [stm32f1xx_hal_rcc.c](stm32f1xx_hal_rcc.c) | Biblioteca ST: árvore de clock e habilitação de periféricos. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `RCC_Delay`, `HAL_RCC_MCOConfig`, `HAL_RCC_EnableCSS`, `HAL_RCC_DisableCSS`, `HAL_RCC_GetSysClockFreq`, `HAL_RCC_GetHCLKFreq`, `HAL_RCC_GetPCLK1Freq`, `HAL_RCC_GetPCLK2Freq`, `HAL_RCC_GetOscConfig`, `HAL_RCC_GetClockConfig`, `HAL_RCC_NMI_IRQHandler`, `HAL_RCC_CSSCallback`. |
| [stm32f1xx_hal_rcc_ex.c](stm32f1xx_hal_rcc_ex.c) | Biblioteca ST: configurações adicionais de clock. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `HAL_RCCEx_GetPeriphCLKConfig`, `HAL_RCCEx_GetPeriphCLKFreq`. |
| [stm32f1xx_hal_tim.c](stm32f1xx_hal_tim.c) | Biblioteca ST: timers, incluindo contador do encoder. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `TIM_OC1_SetConfig`, `TIM_OC3_SetConfig`, `TIM_OC4_SetConfig`, `TIM_TI1_ConfigInputStage`, `TIM_TI2_SetConfig`, `TIM_TI2_ConfigInputStage`, `TIM_TI3_SetConfig`, `TIM_TI4_SetConfig`, `TIM_ITRx_SetConfig`, `TIM_DMAPeriodElapsedCplt`, `TIM_DMAPeriodElapsedHalfCplt`, `TIM_DMADelayPulseCplt`. |
| [stm32f1xx_hal_tim_ex.c](stm32f1xx_hal_tim_ex.c) | Biblioteca ST: recursos adicionais dos timers. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `TIM_DMADelayPulseNCplt`, `TIM_DMAErrorCCxN`, `TIM_CCxNChannelCmd`, `HAL_TIMEx_HallSensor_MspInit`, `HAL_TIMEx_HallSensor_MspDeInit`, `HAL_TIMEx_CommutCallback`, `HAL_TIMEx_CommutHalfCpltCallback`, `HAL_TIMEx_BreakCallback`, `TIMEx_DMACommutationCplt`, `TIMEx_DMACommutationHalfCplt`. |

## Observações para manutenção

Conteúdo de terceiro: preserve a licença e a documentação upstream. Mudanças no pacote exigem revisar includes, configuração e compatibilidade das interfaces.

[Voltar à pasta superior](../README.md)
