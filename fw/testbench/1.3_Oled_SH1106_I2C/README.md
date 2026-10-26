# 1.3_Oled_SH1106_I2C - guia de manutenção

Projeto de referência do OLED/SH1106, com arquivos STM32F1. Não substituir diretamente a pinagem/HAL do projeto F407.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Subpastas

| Pasta | Responsabilidade |
|---|---|
| [1.3_Oled_SH1106_I2C](1.3_Oled_SH1106_I2C/README.md) | Projeto de referência do OLED/SH1106, com arquivos STM32F1. Não substituir diretamente a pinagem/HAL do projeto F407. |

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| Sem arquivos diretos | Consulte os READMEs das subpastas acima. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
