# Application - guia de manutenção

Projeto manual de validação de drivers e comunicação. O estado local inclui diagnóstico SD somente de leitura e pode divergir da versão oficial entregue.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Subpastas

| Pasta | Responsabilidade |
|---|---|
| [board](board/README.md) | Projeto manual de validação de drivers e comunicação. O estado local inclui diagnóstico SD somente de leitura e pode divergir da versão oficial entregue. |
| [core](core/README.md) | Projeto manual de validação de drivers e comunicação. O estado local inclui diagnóstico SD somente de leitura e pode divergir da versão oficial entregue. |
| [drivers](drivers/README.md) | Projeto manual de validação de drivers e comunicação. O estado local inclui diagnóstico SD somente de leitura e pode divergir da versão oficial entregue. |
| [interfaces](interfaces/README.md) | Projeto manual de validação de drivers e comunicação. O estado local inclui diagnóstico SD somente de leitura e pode divergir da versão oficial entregue. |
| [tests](tests/README.md) | Projeto manual de validação de drivers e comunicação. O estado local inclui diagnóstico SD somente de leitura e pode divergir da versão oficial entregue. |

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [app.c](app.c) | Orquestra inicialização lógica, leituras periódicas, comandos da interface e avanço das sessões. É o ponto de ligação entre UI, estado e drivers. Entradas/símbolos para consulta: `safe`, `command`, `draw`, `app_init`, `app_run`. |
| [app.h](app.h) | Orquestra inicialização lógica, leituras periódicas, comandos da interface e avanço das sessões. É o ponto de ligação entre UI, estado e drivers. Entradas/símbolos para consulta: `app_init`, `app_run`. |
| [bench_config.h](bench_config.h) | Habilitações e parâmetros específicos dos testes manuais de hardware. |
| [config.h](config.h) | Limites e parâmetros gerais do protocolo: temperaturas, quantidade de pontos, aquisição e timeouts. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
