# Adaptação da NUCLEO-H753ZI

GPIOs, clocks, temporização e interfaces concretas com HAL. Única camada que deve concentrar detalhes da placa e o estado seguro dos atuadores.

Estado: estrutura inicial criada em 26/10/2026.

## Implementações previstas

`board.c/.h`, `pinout.h`, `runtime_config.h`, temporização por timer/DWT e adaptação ADC/DMA. Separar buffers por memória acessível ao DMA e definir política D-Cache/MPU. Os nomes de pinos da POC são referências históricas, não uma definição do MVP.

## Manutenção

Implementações futuras devem manter as responsabilidades desta camada e atualizar este README com os arquivos efetivamente criados. Arquivos de outras gerações não devem ser copiados sem revisão de interfaces e premissas.
