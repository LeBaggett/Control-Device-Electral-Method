# Thermal Analyzer Web

Nova interface acessada por Ethernet no navegador, mantendo as funções analíticas da aplicação Python como referência.

Estado: estrutura inicial criada em 26/10/2026.

## Organização

`src/domain` contém cálculos puros; `src/services` recebe dados do dispositivo; `src/state` mantém a sessão; `src/pages` e `src/components` apresentam telas e gráficos; `public` reúne recursos estáticos locais.

Ainda não há framework, servidor ou bundle executável definido. A intenção é servir recursos estáticos pelo STM32; o navegador desenha gráficos e exporta relatórios. O Analyzer Python da POC permanece em `../thermal_analyzer`.

Não depender de CDN para a operação básica. Testar equivalência numérica entre os cálculos portados e os exemplos do Analyzer. Nenhum estado do navegador deve comandar diretamente o instante de captura do ADC.

## Subpastas

- [docs](docs/README.md)
- [public](public/README.md)
- [src](src/README.md)
- [tests](tests/README.md)

## Manutenção

Implementações futuras devem manter as responsabilidades desta camada e atualizar este README com os arquivos efetivamente criados. Arquivos de outras gerações não devem ser copiados sem revisão de interfaces e premissas.
