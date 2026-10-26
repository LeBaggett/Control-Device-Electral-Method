# Application - guia de manutenção

Código próprio do firmware oficial. app coordena board, drivers, core e UI; configuração central em config.h e board/runtime_config.h.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Subpastas

| Pasta | Responsabilidade |
|---|---|
| [board](board/README.md) | Camada de adaptação STM32/placa: pinagem, HAL, estado seguro, relógio e comunicação. Centraliza os detalhes físicos consumidos pelos drivers. |
| [core](core/README.md) | Estado e algoritmos das sessões: calibração elétrica, ensaio por alvos, K manual, termopar e TSP. Manter unidades e sequência de potência explícitas. |
| [docs](docs/README.md) | Notas técnicas de integração e manutenção. Conferir o código atual quando houver diferenças entre documentos de etapas anteriores. |
| [drivers](drivers/README.md) | Drivers e serviços da aplicação: ADC/Vf, termopar, potência, OLED, encoder e armazenamento. Headers expõem contratos para UI/app/testes. |
| [interfaces](interfaces/README.md) | Contratos compartilhados sem detalhes da placa, como eventos do encoder. |
| [tests](tests/README.md) | Testes no computador com hardware simulado/stubs. Usam GCC/Python e verificam contratos; não devem ser incluídos na imagem STM32. |
| [ui](ui/README.md) | Interface OLED, navegação e recursos gráficos. Os comandos devem ser tratados pela orquestração; desenho de tela não substitui guardas do protocolo. |

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [app.c](app.c) | Orquestra inicialização lógica, leituras periódicas, comandos da interface e avanço das sessões. É o ponto de ligação entre UI, estado e drivers. Entradas/símbolos para consulta: `app_update_measurements`, `tc_sync`, `tc_apply_saved`, `tc_save`, `tc_heat_sync`, `tc_accept_point`, `tc_collect`, `app_start_command`, `app_init`, `app_run`. |
| [app.h](app.h) | Orquestra inicialização lógica, leituras periódicas, comandos da interface e avanço das sessões. É o ponto de ligação entre UI, estado e drivers. Entradas/símbolos para consulta: `app_init`, `app_run`. |
| [config.h](config.h) | Limites e parâmetros gerais do protocolo: temperaturas, quantidade de pontos, aquisição e timeouts. |
| [main.c.example](main.c.example) | Arquivo de suporte main.c.example; identificar seu consumidor antes de modificar ou remover. |
| [Makefile](Makefile) | Regras de compilação/testes. Identifica fontes, includes, ferramentas e alvos deste diretório. |
| [ORIGEM.txt](ORIGEM.txt) | Arquivo de suporte/documentação/configuração ORIGEM.txt; conferir as referências locais antes de editar. |
| [target_sources.list](target_sources.list) | Arquivo de suporte/documentação/configuração target_sources.list; conferir as referências locais antes de editar. |
| [VALIDACAO.md](VALIDACAO.md) | Documento específico: validacao. Conferir a data/contexto e confrontar afirmações históricas com o código entregue. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
