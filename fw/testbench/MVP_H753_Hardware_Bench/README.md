# Testbench MVP H753 - etapa 1: base + Ethernet

Projeto para **NUCLEO-H753ZI / MB1364**, sem RTOS. Esta entrega contém fontes, HAL/CMSIS/LwIP/PHY com versões fixadas e metadados de projeto CubeIDE. **Compilação e testes no computador foram executados; não houve gravação nem validação física da placa.**

## O que está implementado

- Inicialização com HSI + PLL: núcleo 400 MHz, barramento HCLK 200 MHz; SysTick em milissegundos.
- LED verde de atividade; amarelo acompanha o estado de link; vermelho indica falha fatal.
- Ethernet RMII, PHY LAN8742 e LwIP `NO_SYS`, sem RTOS.
- Transmissão Ethernet por DMA/interrupção e liberação de buffers no superloop.
- Recepção atendida com orçamento de quatro quadros por passagem, além dos timeouts LwIP.
- IPv4 fixo **192.168.7.2/24**, sem DHCP, DNS ou gateway nesta bancada.
- Página de diagnóstico e endpoint somente leitura **`GET /api/status`**.
- Diagnósticos em **`g_bench_debug`**, inclusive erros HAL/DMA e mudança de link.

RS-485, IHM, ADC/timer do TSP, fonte, relé/SELECT e aquecedor **não estão implementados nesta etapa**. A bancada não configura pinos de atuação; para este teste use somente a NUCLEO, sem shield/DUT/estágio de potência conectado. A ausência de comandos não equivale a uma inibição elétrica de um circuito externo.

## Estratégia e motivos

| Decisão | Por quê | Como verificar |
|---|---|---|
| Base + Ethernet antes de RS-485/ADC | Localiza problemas de clock, memória e rede sem misturá-los com medição. | Heartbeat, status e contadores antes da expansão. |
| Clock HSI/400 MHz | Evita depender do MCO do ST-LINK e não exige 480 MHz para esta função. | `core_hz=400000000`, `hclk_hz=200000000`. Clock final de metrologia ainda será revisto. |
| IP fixo e sem gateway | Elimina DHCP/roteador como variável no cabo direto PC/placa. | PC e placa na mesma sub-rede, ping e HTTP. |
| Página servida pela placa | Testa o transporte real; não requer aplicação Python nem internet. | Abrir 192.168.7.2 e observar respostas avançando. |
| HTTPD oficial LwIP | Evita manter um parser TCP/HTTP próprio nesta fase. | Testes com a implementação real de arquivos HTTP do LwIP. |
| API somente leitura | Diagnóstico não deve acionar potência. | Sem endpoints de comando e sem driver de potência no build. |
| TX DMA/interrupção, processamento no mainloop | Não esperar transmissão por polling nem chamar LwIP de uma ISR. | Contadores TX/RX e falhas; inspeção do código e testes. |
| Quatro quadros por serviço | Tráfego contínuo não deve drenar RX indefinidamente em uma única chamada. | Maior intervalo do loop, depois teste integrado. |
| D-cache OFF, I-cache ON | Primeiro isola coerência de cache como causa de corrupção DMA. Não é a configuração final otimizada. | A configuração está explícita em main.c. Validar MPU/cache em etapa separada. |
| Buffers Ethernet em SRAM D2 | Ethernet DMA não deve usar DTCM; memória tem tamanho/endereço reservados no linker. | Descritores/pool em 0x30000000, heap LwIP em 0x30008000. |
| Dependências versionadas | Permite reproduzir a compilação sem depender do pacote global instalado. | `DEPENDENCIES.json`, `PHY_VERSION.txt` e licenças originais. |

## Primeiro teste na placa

1. Use a NUCLEO-H753ZI, sem hardware de potência ou DUT. Confirme a revisão MB1364. Não use os pinos da POC como ligação do MVP.
2. Importe esta pasta por **File > Import > General > Existing Projects into Workspace** no CubeIDE. O nome do projeto é `MVP_H753_Hardware_Bench`.
3. Execute **Clean > Build**. O ELF do IDE fica em `Debug/MVP_H753_Hardware_Bench.elf`. O projeto já contém `.project/.cproject` com MCU ARM GCC/H753.
4. Crie a configuração de debug STM32 C/C++ Application usando esse ELF e ST-LINK. Grave e deixe executar; pausar a CPU interrompe o atendimento da rede.
5. Alimente/programa pelo ST-LINK e conecte o RJ45 Ethernet da NUCLEO ao PC. São conexões diferentes.
6. Na interface Ethernet do PC, configure temporariamente **192.168.7.1**, máscara **255.255.255.0**, gateway/DNS vazios. Não altere o Wi-Fi. Se a sub-rede conflitar com outra conexão, escolha outra em `Application/bench_config.h` e recompile.
7. Execute `ping 192.168.7.2` e abra **http://192.168.7.2/**. A página deve mostrar dados atualizados a cada aproximadamente um segundo, sem necessidade de internet.
8. Confira heartbeat verde (~500 ms por troca), link amarelo e ausência de LED vermelho. LD1 em PB0 depende de SB39 ON / SB47 OFF. Link Ethernet depende de JP6 e das pontes indicadas em UM2407.
9. Deixe alguns minutos, abra/feche a página e registre os diagnósticos. Depois retire o cabo e reconecte; a recuperação sem reset é um critério a testar, não uma validação já feita.

Não gerar código CubeMX sobre esta pasta nesta etapa: este primeiro projeto é independente, sem `.ioc`. A instalação do STM32CubeH7 pode continuar; a migração para uma geração CubeMX será uma tarefa controlada, comparando startup, pinagem, memória e ethernetif. Gerar por cima pode remover as escolhas de diagnóstico.

## Compilação alternativa e testes

Com Python 3 e GCC ARM disponível (ou variável `ARM_GCC` apontando para o executável):

```text
python tools/build.py
python Application/tests/run_tests.py
```

O build alternativo produz `Build/MVP_H753_Hardware_Bench.elf` e `.bin`, com otimização de depuração `-Og`. O CubeIDE usa sua configuração Debug; os tamanhos e binários podem diferir. Ambos foram compilados para Cortex-M7/H753.

Para editar a página: altere `Application/ui/index.html`, execute `python tools/embed_web.py` e recompile. Não edite o array gerado manualmente.

## Diagnóstico para enviar

Após a primeira execução, envie:

```text
g_bench_debug.boot_stage
g_bench_debug.fatal_code
g_bench_debug.core_hz
g_bench_debug.eth_hal_status
g_bench_debug.phy_status
g_bench_debug.link_up
g_bench_debug.http_requests
```

Esperados: boot_stage 5 (aplicação/servidor iniciados), fatal_code 0, core_hz 400000000, HAL/PHY 0 e link_up 1 com conexão negociada. boot_stage 5 sozinho não comprova link. Falha fatal coloca estágio 99 e retém CFSR/HFSR/BFAR/MMFAR quando aplicável.

`max_gap_ms` mede intervalos do superloop com resolução de milissegundos; não mede jitter de microssegundos nem prova precisão de ADC. Códigos PHY negativos aparecem no campo de 32 bits do depurador; interpretar como int32 quando necessário.

## Critérios e limites

Consulte [VALIDACAO.md](VALIDACAO.md). A próxima etapa só adiciona RS-485 após comprovar base/Ethernet. Depois vem aquisição de tensão conhecida, e só então teste simultâneo das três interfaces.

Referências: [UM2407](https://www.st.com/resource/en/user_manual/um2407-stm32h7-nucleo144-boards-mb1364-stmicroelectronics.pdf) e exemplo oficial [LwIP HTTP Raw da ST](https://github.com/STMicroelectronics/STM32CubeH7/tree/master/Projects/STM32H743I-EVAL/Applications/LwIP/LwIP_HTTP_Server_Raw). O ethernetif foi adaptado; não é o exemplo EVAL sem alterações.
