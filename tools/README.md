# tools - guia de manutenção

Ferramentas executadas no computador, separadas da imagem gravada no STM32.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Subpastas

| Pasta | Responsabilidade |
|---|---|
| [thermal_analyzer](thermal_analyzer/README.md) | Thermal Analyzer: aplicação Python/Tkinter para importar CAL/TST/TSP, visualizar medições e gerar relatórios; não controla a potência do dispositivo. |

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [thermal_analyzer.zip](thermal_analyzer.zip) | Arquivo de suporte thermal_analyzer.zip; identificar seu consumidor antes de modificar ou remover. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
