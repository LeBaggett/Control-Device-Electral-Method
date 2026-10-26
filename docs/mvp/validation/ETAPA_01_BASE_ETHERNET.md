# Etapa 1 - base H753 e Ethernet

Data: 26/10/2026. Projeto: [MVP_H753_Hardware_Bench](../../../fw/testbench/MVP_H753_Hardware_Bench/README.md).

## Estratégia

Validar a base e a rede antes de adicionar RS-485 ou aquisição. Hardware de potência/DUT permanece desconectado. O firmware configura somente recursos da NUCLEO usados na bancada; não implementa comando de potência.

## Decisões e motivos

- Sem RTOS: LwIP NO_SYS e máquina de serviço no superloop.
- HSI/PLL em 400 MHz: independe do MCO de depuração; clock final de aquisição será revisto.
- IP fixo 192.168.7.2/24: permite cabo direto, eliminando DHCP/roteador do primeiro diagnóstico.
- HTTPD LwIP e API somente leitura: reutiliza a pilha existente e permite observar o estado sem acionar hardware.
- Ethernet TX por DMA/interrupção; manipulação LwIP/buffers no superloop.
- Recepção com orçamento: evita drenar uma fila continuamente em uma única chamada.
- D-cache temporariamente desligado: isola problemas de coerência durante bring-up. I-cache habilitado; descritores e memória Ethernet em SRAM D2, nunca DTCM.
- Dependências oficiais com commits registrados: torna o teste independente da instalação global do pacote H7.

## Evidência disponível

Compilação/vinculação ARM e importação/build em workspace temporário do STM32CubeIDE 1.19.0 (0 erros, 0 avisos no build concluído). Testes C com fs.c real do LwIP cobriram snapshots HTTP independentes, JSON/tamanho, falta de memória, wrap e contadores. Nenhuma placa foi gravada nesta atividade.

## O que o responsável deve validar

Boot/LEDs, modelo e pontes da placa, link/ping, página atualizando, retirada/reconexão do cabo e operação contínua. Resultados físicos permanecem pendentes. Procedimento e expressões em [README da bancada](../../../fw/testbench/MVP_H753_Hardware_Bench/README.md); ficha em [VALIDACAO](../../../fw/testbench/MVP_H753_Hardware_Bench/VALIDACAO.md).

## Limites desta entrega

Não há RS-485, termopares, IHM, ADC/TSP ou atuadores no build. Não há .ioc: não gerar CubeMX sobre esta bancada sem uma etapa explícita de migração. Precisão de microsegundos e estabilidade Ethernet só serão confirmadas por medição na placa.

Próxima etapa: após a aprovação física da base/Ethernet, integrar uma transação RS-485 por vez. A aquisição e o teste simultâneo vêm depois.
