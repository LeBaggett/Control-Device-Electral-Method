# Inc - guia de manutenção

Bibliotecas de periféricos do fabricante. Manter fontes e headers da mesma versão; registrar qualquer ajuste local antes de atualizar o pacote.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Subpastas

| Pasta | Responsabilidade |
|---|---|
| [Legacy](Legacy/README.md) | Bibliotecas de periféricos do fabricante. Manter fontes e headers da mesma versão; registrar qualquer ajuste local antes de atualizar o pacote. |

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [stm32f1xx_hal.h](stm32f1xx_hal.h) | Interface C, tipos e constantes do módulo stm32f1xx_hal. Conferir includes e os consumidores antes de mudar assinaturas. Entradas/símbolos para consulta: `HAL_MspInit`, `HAL_MspDeInit`, `HAL_IncTick`, `HAL_Delay`, `HAL_GetTick`, `HAL_GetTickPrio`, `HAL_SuspendTick`, `HAL_ResumeTick`, `HAL_GetHalVersion`, `HAL_GetREVID`, `HAL_GetDEVID`, `HAL_GetUIDw0`. |
| [stm32f1xx_hal_cortex.h](stm32f1xx_hal_cortex.h) | Biblioteca ST: interrupções e recursos do núcleo. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `HAL_NVIC_SetPriorityGrouping`, `HAL_NVIC_SetPriority`, `HAL_NVIC_EnableIRQ`, `HAL_NVIC_DisableIRQ`, `HAL_NVIC_SystemReset`, `HAL_SYSTICK_Config`, `HAL_NVIC_GetPriorityGrouping`, `HAL_NVIC_GetPriority`, `HAL_NVIC_GetPendingIRQ`, `HAL_NVIC_SetPendingIRQ`, `HAL_NVIC_ClearPendingIRQ`, `HAL_NVIC_GetActive`. |
| [stm32f1xx_hal_def.h](stm32f1xx_hal_def.h) | Biblioteca ST: def. Header e fonte devem permanecer compatíveis com a versão do pacote. |
| [stm32f1xx_hal_dma.h](stm32f1xx_hal_dma.h) | Biblioteca ST: transferências DMA. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `HAL_DMA_IRQHandler`, `HAL_DMA_GetError`. |
| [stm32f1xx_hal_dma_ex.h](stm32f1xx_hal_dma_ex.h) | Biblioteca ST: recursos adicionais de DMA. Header e fonte devem permanecer compatíveis com a versão do pacote. |
| [stm32f1xx_hal_exti.h](stm32f1xx_hal_exti.h) | Biblioteca ST: exti. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `HAL_EXTI_IRQHandler`, `HAL_EXTI_GetPending`, `HAL_EXTI_ClearPending`, `HAL_EXTI_GenerateSWI`. |
| [stm32f1xx_hal_flash.h](stm32f1xx_hal_flash.h) | Biblioteca ST: memória Flash. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `HAL_FLASH_IRQHandler`, `HAL_FLASH_EndOfOperationCallback`, `HAL_FLASH_OperationErrorCallback`, `HAL_FLASH_OB_Launch`, `HAL_FLASH_GetError`. |
| [stm32f1xx_hal_flash_ex.h](stm32f1xx_hal_flash_ex.h) | Biblioteca ST: operações adicionais de Flash. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `HAL_FLASHEx_OBGetConfig`, `HAL_FLASHEx_OBGetUserData`. |
| [stm32f1xx_hal_gpio.h](stm32f1xx_hal_gpio.h) | Biblioteca ST: GPIOs e funções alternativas. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `HAL_GPIO_Init`, `HAL_GPIO_DeInit`, `HAL_GPIO_WritePin`, `HAL_GPIO_TogglePin`, `HAL_GPIO_EXTI_IRQHandler`, `HAL_GPIO_EXTI_Callback`. |
| [stm32f1xx_hal_gpio_ex.h](stm32f1xx_hal_gpio_ex.h) | Biblioteca ST: gpio ex. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `HAL_GPIOEx_ConfigEventout`, `HAL_GPIOEx_EnableEventout`, `HAL_GPIOEx_DisableEventout`. |
| [stm32f1xx_hal_i2c.h](stm32f1xx_hal_i2c.h) | Biblioteca ST: comunicação I2C do OLED. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `HAL_I2C_MspInit`, `HAL_I2C_MspDeInit`, `HAL_I2C_EV_IRQHandler`, `HAL_I2C_ER_IRQHandler`, `HAL_I2C_MasterTxCpltCallback`, `HAL_I2C_MasterRxCpltCallback`, `HAL_I2C_SlaveTxCpltCallback`, `HAL_I2C_SlaveRxCpltCallback`, `HAL_I2C_AddrCallback`, `HAL_I2C_ListenCpltCallback`, `HAL_I2C_MemTxCpltCallback`, `HAL_I2C_MemRxCpltCallback`. |
| [stm32f1xx_hal_pwr.h](stm32f1xx_hal_pwr.h) | Biblioteca ST: controle de energia do MCU. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `HAL_PWR_DeInit`, `HAL_PWR_EnableBkUpAccess`, `HAL_PWR_DisableBkUpAccess`, `HAL_PWR_ConfigPVD`, `HAL_PWR_EnablePVD`, `HAL_PWR_DisablePVD`, `HAL_PWR_EnableWakeUpPin`, `HAL_PWR_DisableWakeUpPin`, `HAL_PWR_EnterSTOPMode`, `HAL_PWR_EnterSLEEPMode`, `HAL_PWR_EnterSTANDBYMode`, `HAL_PWR_EnableSleepOnExit`. |
| [stm32f1xx_hal_rcc.h](stm32f1xx_hal_rcc.h) | Biblioteca ST: árvore de clock e habilitação de periféricos. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `HAL_RCC_MCOConfig`, `HAL_RCC_EnableCSS`, `HAL_RCC_DisableCSS`, `HAL_RCC_GetSysClockFreq`, `HAL_RCC_GetHCLKFreq`, `HAL_RCC_GetPCLK1Freq`, `HAL_RCC_GetPCLK2Freq`, `HAL_RCC_GetOscConfig`, `HAL_RCC_GetClockConfig`, `HAL_RCC_NMI_IRQHandler`, `HAL_RCC_CSSCallback`. |
| [stm32f1xx_hal_rcc_ex.h](stm32f1xx_hal_rcc_ex.h) | Biblioteca ST: configurações adicionais de clock. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `HAL_RCCEx_GetPeriphCLKConfig`, `HAL_RCCEx_GetPeriphCLKFreq`. |
| [stm32f1xx_hal_tim.h](stm32f1xx_hal_tim.h) | Biblioteca ST: timers, incluindo contador do encoder. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `HAL_TIM_Base_MspInit`, `HAL_TIM_Base_MspDeInit`, `HAL_TIM_OC_MspInit`, `HAL_TIM_OC_MspDeInit`, `HAL_TIM_PWM_MspInit`, `HAL_TIM_PWM_MspDeInit`, `HAL_TIM_IC_MspInit`, `HAL_TIM_IC_MspDeInit`, `HAL_TIM_OnePulse_MspInit`, `HAL_TIM_OnePulse_MspDeInit`, `HAL_TIM_Encoder_MspInit`, `HAL_TIM_Encoder_MspDeInit`. |
| [stm32f1xx_hal_tim_ex.h](stm32f1xx_hal_tim_ex.h) | Biblioteca ST: recursos adicionais dos timers. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `HAL_TIMEx_HallSensor_MspInit`, `HAL_TIMEx_HallSensor_MspDeInit`, `HAL_TIMEx_CommutCallback`, `HAL_TIMEx_CommutHalfCpltCallback`, `HAL_TIMEx_BreakCallback`, `TIMEx_DMACommutationCplt`, `TIMEx_DMACommutationHalfCplt`. |
| [stm32f1xx_ll_bus.h](stm32f1xx_ll_bus.h) | Biblioteca ST: bus. Header e fonte devem permanecer compatíveis com a versão do pacote. |
| [stm32f1xx_ll_cortex.h](stm32f1xx_ll_cortex.h) | Biblioteca ST: interrupções e recursos do núcleo. Header e fonte devem permanecer compatíveis com a versão do pacote. |
| [stm32f1xx_ll_dma.h](stm32f1xx_ll_dma.h) | Biblioteca ST: transferências DMA. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `LL_DMA_Init`, `LL_DMA_DeInit`, `LL_DMA_StructInit`. |
| [stm32f1xx_ll_exti.h](stm32f1xx_ll_exti.h) | Biblioteca ST: exti. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `LL_EXTI_Init`, `LL_EXTI_DeInit`, `LL_EXTI_StructInit`. |
| [stm32f1xx_ll_gpio.h](stm32f1xx_ll_gpio.h) | Biblioteca ST: GPIOs e funções alternativas. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `LL_GPIO_StructInit`. |
| [stm32f1xx_ll_i2c.h](stm32f1xx_ll_i2c.h) | Biblioteca ST: comunicação I2C do OLED. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `LL_I2C_Init`, `LL_I2C_DeInit`, `LL_I2C_StructInit`. |
| [stm32f1xx_ll_pwr.h](stm32f1xx_ll_pwr.h) | Biblioteca ST: controle de energia do MCU. Header e fonte devem permanecer compatíveis com a versão do pacote. |
| [stm32f1xx_ll_rcc.h](stm32f1xx_ll_rcc.h) | Biblioteca ST: árvore de clock e habilitação de periféricos. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `LL_RCC_GetSystemClocksFreq`, `LL_RCC_GetI2SClockFreq`, `LL_RCC_GetUSBClockFreq`, `LL_RCC_GetADCClockFreq`. |
| [stm32f1xx_ll_system.h](stm32f1xx_ll_system.h) | Biblioteca ST: system. Header e fonte devem permanecer compatíveis com a versão do pacote. |
| [stm32f1xx_ll_utils.h](stm32f1xx_ll_utils.h) | Biblioteca ST: utils. Header e fonte devem permanecer compatíveis com a versão do pacote. Entradas/símbolos para consulta: `LL_Init1msTick`, `LL_mDelay`, `LL_SetSystemCoreClock`. |

## Observações para manutenção

Conteúdo de terceiro: preserve a licença e a documentação upstream. Mudanças no pacote exigem revisar includes, configuração e compatibilidade das interfaces.

[Voltar à pasta superior](../README.md)
