# Target - guia de manutenção

Projeto manual de validação de drivers e comunicação. O estado local inclui diagnóstico SD somente de leitura e pode divergir da versão oficial entregue.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [bsp_driver_sd.c](bsp_driver_sd.c) | Ponte BSP → HAL SD: transferências e captura dos erros/registros antes que consultas de estado os sobrescrevam. Entradas/símbolos para consulta: `BSP_SD_Init`, `BSP_SD_ITConfig`, `BSP_SD_DetectIT`, `sd_wait_transfer`, `sd_sector_all_zero`, `BSP_SD_ReadBlocks`, `BSP_SD_WriteBlocks`, `BSP_SD_ReadBlocks_DMA`, `BSP_SD_WriteBlocks_DMA`, `BSP_SD_Erase`, `BSP_SD_GetCardState`, `BSP_SD_GetCardInfo`. |
| [bsp_driver_sd.h](bsp_driver_sd.h) | Ponte BSP → HAL SD: transferências e captura dos erros/registros antes que consultas de estado os sobrescrevam. Entradas/símbolos para consulta: `BSP_SD_Init`, `BSP_SD_ITConfig`, `BSP_SD_DetectIT`, `BSP_SD_DetectCallback`, `BSP_SD_ReadBlocks`, `BSP_SD_WriteBlocks`, `BSP_SD_ReadBlocks_DMA`, `BSP_SD_WriteBlocks_DMA`, `BSP_SD_Erase`, `BSP_SD_IRQHandler`, `BSP_SD_DMA_Tx_IRQHandler`, `BSP_SD_DMA_Rx_IRQHandler`. |
| [ffconf.h](ffconf.h) | Opções de compilação FatFs: suporte exFAT, nomes, código de página, volumes e recursos disponíveis. |
| [sd_diskio.c](sd_diskio.c) | Ponte FatFs → BSP: inicialização/status/read/write/ioctl e espera pelo fim das transferências. |
| [sd_diskio.h](sd_diskio.h) | Ponte FatFs → BSP: inicialização/status/read/write/ioctl e espera pelo fim das transferências. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
