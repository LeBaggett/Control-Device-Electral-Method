# ui - guia de manutenção

Interface OLED, navegação e recursos gráficos. Os comandos devem ser tratados pela orquestração; desenho de tela não substitui guardas do protocolo.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [assets.c](assets.c) | Recursos gráficos e utilitários usados pelas telas. |
| [assets.h](assets.h) | Recursos gráficos e utilitários usados pelas telas. |
| [epd_bitmap_.bin](epd_bitmap_.bin) | Recurso visual/binário; conferir consumidor, dimensões e formato antes de substituir. |
| [logo.h](logo.h) | Arte original da splash incorporada como bitmap; manter dimensões e ordem de bits. |
| [ui.c](ui.c) | Navegação, edição e renderização das telas do projeto principal; emite comandos para app em vez de executar o protocolo diretamente. Entradas/símbolos para consulta: `ui_current_screen`, `ui_init`, `go`, `rotate`, `ui_step`, `title`, `number`, `choice`, `ui_render`. |
| [ui.h](ui.h) | Navegação, edição e renderização das telas do projeto principal; emite comandos para app em vez de executar o protocolo diretamente. Entradas/símbolos para consulta: `ui_init`, `ui_step`, `ui_render`, `ui_current_screen`. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
