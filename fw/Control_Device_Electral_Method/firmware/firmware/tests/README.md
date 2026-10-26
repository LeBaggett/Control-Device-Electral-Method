# tests - guia de manutenção

Estrutura auxiliar/legada de testes. O projeto ativo é Core + Application + FATFS + Drivers + Middlewares; conferir .cproject antes de adicionar fontes desta pasta ao build.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Subpastas

| Pasta | Responsabilidade |
|---|---|
| [vendor](vendor/README.md) | Dependência Unity e seus exemplos/testes auxiliares. Não faz parte do firmware gravado; preservar documentação e licença originais. |

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| Sem arquivos diretos | Consulte os READMEs das subpastas acima. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
