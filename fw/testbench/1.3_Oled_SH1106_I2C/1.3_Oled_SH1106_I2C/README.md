# 1.3_Oled_SH1106_I2C - guia de manutenção

Projeto de referência do OLED/SH1106, com arquivos STM32F1. Não substituir diretamente a pinagem/HAL do projeto F407.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Subpastas

| Pasta | Responsabilidade |
|---|---|
| [Core](Core/README.md) | Projeto de referência do OLED/SH1106, com arquivos STM32F1. Não substituir diretamente a pinagem/HAL do projeto F407. |
| [Drivers](Drivers/README.md) | Bibliotecas de periféricos do fabricante. Manter fontes e headers da mesma versão; registrar qualquer ajuste local antes de atualizar o pacote. |

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [.cproject](.cproject) | Metadados Eclipse/CubeIDE: identidade do projeto, toolchain, includes e exclusões de compilação. |
| [.DS_Store](.DS_Store) | Arquivo de suporte .DS_Store; identificar seu consumidor antes de modificar ou remover. |
| [.mxproject](.mxproject) | Arquivo de suporte .mxproject; identificar seu consumidor antes de modificar ou remover. |
| [.project](.project) | Metadados Eclipse/CubeIDE: identidade do projeto, toolchain, includes e exclusões de compilação. |
| [1.3_Oled_SH1106_I2C Debug.launch](1.3_Oled_SH1106_I2C%20Debug.launch) | Configuração de execução/depuração do CubeIDE. Conferir ELF, ST-LINK e portas locais antes de reutilizar. |
| [1.3_Oled_SH1106_I2C.ioc](1.3_Oled_SH1106_I2C.ioc) | Projeto CubeMX: pinos, clocks, periféricos e gerador. Regenerar pode sobrescrever código fora de USER CODE. |
| [STM32F103C8TX_FLASH.ld](STM32F103C8TX_FLASH.ld) | Script de ligação: regiões Flash/RAM, seções, heap e stack. Deve corresponder ao MCU e ao startup. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
