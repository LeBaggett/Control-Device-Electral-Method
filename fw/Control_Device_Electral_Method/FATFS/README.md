# FATFS - guia de manutenção

Integração CubeMX/FatFs: registro do volume, configuração do filesystem e ponte de operações para BSP/HAL SD.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Subpastas

| Pasta | Responsabilidade |
|---|---|
| [App](App/README.md) | Integração CubeMX/FatFs: registro do volume, configuração do filesystem e ponte de operações para BSP/HAL SD. |
| [Target](Target/README.md) | Integração CubeMX/FatFs: registro do volume, configuração do filesystem e ponte de operações para BSP/HAL SD. |

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| Sem arquivos diretos | Consulte os READMEs das subpastas acima. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
