# App - guia de manutenção

Integração CubeMX/FatFs: registro do volume, configuração do filesystem e ponte de operações para BSP/HAL SD.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [fatfs.c](fatfs.c) | Registro do driver de disco e identificação do volume usado pelo FatFs. Entradas/símbolos para consulta: `MX_FATFS_Init`. |
| [fatfs.h](fatfs.h) | Registro do driver de disco e identificação do volume usado pelo FatFs. Entradas/símbolos para consulta: `MX_FATFS_Init`. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
