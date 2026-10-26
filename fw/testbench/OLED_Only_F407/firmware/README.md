# firmware - guia de manutenção

Estrutura auxiliar/legada de testes. O projeto ativo é Core + Application + FATFS + Drivers + Middlewares; conferir .cproject antes de adicionar fontes desta pasta ao build.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [OLED_Only_F407.bin](OLED_Only_F407.bin) | Recurso visual/binário; conferir consumidor, dimensões e formato antes de substituir. |
| [OLED_Only_F407.elf](OLED_Only_F407.elf) | Arquivo de suporte OLED_Only_F407.elf; identificar seu consumidor antes de modificar ou remover. |
| [OLED_Only_F407.map](OLED_Only_F407.map) | Arquivo de suporte OLED_Only_F407.map; identificar seu consumidor antes de modificar ou remover. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
