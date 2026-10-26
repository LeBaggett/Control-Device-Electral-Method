# Startup - guia de manutenção

Projeto isolado de diagnóstico do OLED no STM32F407. Não executa o ensaio térmico completo.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [startup_stm32f407vetx.s](startup_stm32f407vetx.s) | Tabela de vetores, Reset_Handler e entrada no runtime C para o MCU indicado no nome. Vinculado ao linker script. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
