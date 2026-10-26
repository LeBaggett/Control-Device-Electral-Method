# Core - guia de manutenção

Entrada, inicializações geradas, handlers e startup do STM32. Preservar USER CODE e revisar o diff após regenerar o .ioc.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Subpastas

| Pasta | Responsabilidade |
|---|---|
| [Inc](Inc/README.md) | Entrada, inicializações geradas, handlers e startup do STM32. Preservar USER CODE e revisar o diff após regenerar o .ioc. |
| [Src](Src/README.md) | Entrada, inicializações geradas, handlers e startup do STM32. Preservar USER CODE e revisar o diff após regenerar o .ioc. |
| [Startup](Startup/README.md) | Entrada, inicializações geradas, handlers e startup do STM32. Preservar USER CODE e revisar o diff após regenerar o .ioc. |

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| Sem arquivos diretos | Consulte os READMEs das subpastas acima. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
