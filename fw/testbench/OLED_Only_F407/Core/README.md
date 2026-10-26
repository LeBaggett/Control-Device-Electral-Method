# Core - guia de manutenção

Projeto isolado de diagnóstico do OLED no STM32F407. Não executa o ensaio térmico completo.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Subpastas

| Pasta | Responsabilidade |
|---|---|
| [Inc](Inc/README.md) | Projeto isolado de diagnóstico do OLED no STM32F407. Não executa o ensaio térmico completo. |
| [Src](Src/README.md) | Projeto isolado de diagnóstico do OLED no STM32F407. Não executa o ensaio térmico completo. |
| [Startup](Startup/README.md) | Projeto isolado de diagnóstico do OLED no STM32F407. Não executa o ensaio térmico completo. |

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| Sem arquivos diretos | Consulte os READMEs das subpastas acima. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
