# Control Device - MVP STM32H753

Branch de desenvolvimento do MVP. **Estado atual: planejamento; não é firmware H753 pronto para gravar.**

O escopo técnico, as decisões confirmadas, as pendências de hardware e as etapas de validação estão em [docs/mvp/README.md](docs/mvp/README.md).

## Direção do projeto

- NUCLEO-H753ZI, IHM DOP-107BV e três termopares configuráveis J/K.
- Comunicação RS-485 com DMA/interrupções e Ethernet direta ao computador.
- Nova fonte/fixture para memórias DDR5/LPDDR5, a dimensionar a partir dos datasheets.
- Calibração com aquecedor externo comandado pelo STM32, ainda a selecionar.
- ENSAIO TSP e interface web local com gráficos/relatórios semelhantes ao Thermal Analyzer.

## Relação com a POC

A POC entregue está na tag [`v1.0.0-poc`](../../releases/tag/v1.0.0-poc) e na branch `poc-stm32f407`. Os arquivos F407 e o Analyzer Python herdados nesta branch são referências para a migração. Não representam suporte H753 já implementado.

O desenvolvimento desta branch não altera a tag da entrega. Manter a POC na `main` até uma futura integração do MVP aprovada.
