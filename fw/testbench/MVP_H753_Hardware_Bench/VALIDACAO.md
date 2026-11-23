# Verificação da entrega e roteiro de bancada

## Verificado no computador em 26/10/2026

- Compilação/vinculação GCC ARM para STM32H753xx/Cortex-M7, sem RTOS.
- Importação do projeto em workspace temporário do STM32CubeIDE 1.19.0 e compilação Debug concluída com 0 erros e 0 avisos.
- Testes C com `fs.c` real do LwIP: abertura da página/API, rota inexistente, conteúdo/tamanho HTTP, JSON e falta de memória.
- Teste de duas respostas simultâneas: cada conexão possui snapshot próprio, sem sobrescrever dados ainda em envio; memória liberada ao fechar.
- Contadores de link, tratamento de wrap do relógio e truncamento de JSON.
- Inspeção das regiões DMA no ELF/map e separação de heap LwIP.
- Verificação de ausência de chamadas de potência/DAC na aplicação e de transmissão ETH por polling.

## Não verificado fisicamente

| Teste | Resultado a preencher |
|---|---|
| Modelo/revisão NUCLEO e pontes correspondentes | Pendente |
| Boot/LED e core_hz na placa | Pendente |
| Negociação 10/100 do PHY | Pendente |
| Ping pelo cabo direto | Pendente |
| Página e /api/status atualizando | Pendente |
| Dez ciclos de retirada/reconexão do cabo, sem reset | Pendente |
| Operação contínua por 30 minutos, sem falha fatal | Pendente |
| Dois clientes/páginas e consumo estável de memória | Pendente |

Registrar versão, fonte de alimentação, topologia, horário, contadores e falhas. Não preencher como aprovado sem executar o teste.

## Fora da entrega original da etapa 1

RS-485/Modbus, IHM, termopares, ADC/timer TSP, corrente, aquecedor, ajuste de cache/MPU final e ensaio térmico. Nenhuma dessas funções foi simulada como se estivesse operando no dispositivo.


## Etapa 2 — N1040i (2026-11-09)

O usuário confirmou acesso HTTP da etapa anterior após correção da manutenção indevida de cache no RX Ethernet. Isso não valida a serial nova.

Testes novos: `Application/tests/run_n1040_tests.py` (codec e estados reais com transporte simulado, 1 e 3 nós). Regressão: `Application/tests/run_tests.py`. Compilação ARM e integração CubeIDE verificadas nesta entrega; consulte INTEGRACAO_N1040I.md.

| Verificação física | Estado |
|---|---|
| Um N1040i: status e PV iguais ao display | Confirmado pelo usuário em 2026-11-17; painel valid=1/online=1 implica ID esperado aceito pelo firmware |
| DE/RE retorna a RX após último stop bit | Não se aplica ao módulo automático utilizado; modo manual ainda não validado |
| Retirar/reconectar serial sem bloquear Ethernet | Pendente |
| Sensor desconectado não resulta em temperatura válida | Pendente |
| Teste contínuo 30 min e duas abas HTTP | Pendente |
| Três indicadores com endereços distintos | Pendente |


## Baseline funcional de bancada — 2026-11-17

Versão do firmware: **0.2.3-n1040i**, fontes no commit **fa68e77**. O usuário confirmou funcionamento após substituir jumpers com mau contato. As evidências foram enviadas pelo usuário; o agente não realizou a montagem nem o ensaio físico.

- NUCLEO-H753ZI, USART2/recepção DMA, módulo RS485 de direção automática, um N1040i no endereço 247, 115200 baud/8N1.
- Ethernet e painel HTTP funcionando simultaneamente à leitura serial.
- Foto `IMG_20261117_131643462_HDR.jpg`: indicador e painel mostram 26,6 °C, com status de leitura válida.
- Captura `codex-clipboard-0b6ca0b7-2e1d-4d61-9ed4-1bd87f70ab24.png`: 29,7 °C, valid=1, online=1, 3746 respostas, CRC=0, exceções=0, 24 timeouts acumulados, um erro UART e um reinício acumulados.
- `uart_error=4` retém o último erro de enquadramento; o contador não significa uma falha continuamente ativa.
- Os contadores incluem a sessão de diagnóstico/montagem. Não foi demonstrado que todos os erros ocorreram antes da troca dos jumpers; reiniciar e acompanhar novos incrementos é parte do teste de estabilidade pendente.
- Esta é uma baseline funcional com **um instrumento**. Não equivale a validação de 30 minutos, teste de reconexão, sensor desconectado ou três indicadores. A sequência reservada continua 247, 246, 245, com count=1 neste firmware.

Nenhuma alteração executável foi necessária para registrar este resultado; este commit atualiza a evidência de bancada e preserva a versão confirmada pelo usuário.


## Três canais habilitados — 2026-11-23

Versão 0.2.4-three-n1040i: count=3, consultas em ordem 247, 246, 245. O código anterior consultava apenas 247, pois count=1 era deliberado durante a validação inicial. A página distingue quantidade configurada de leituras válidas; não faz descoberta automática.

Teste de regressão simula também o indicador 246 ausente e verifica que 247 e 245 continuam atendidos. Esse teste não substitui a verificação da bancada com os três equipamentos físicos. UART4/IHM ainda não é inicializada: falta confirmar pinos na NUCLEO e parâmetros/projeto DOPSoft. A cadeia física informada pelo usuário é IHM → DB9/bornes → módulo RS485/TTL independente → UART4.


## Perfil rápido experimental — 2026-11-23

Versão 0.2.5-poll-test, consultas de 25 ms e atualização das temperaturas na página com intervalo de 250 ms após cada fetch. Testes em computador cobrem perfis rápido/anterior e respostas lentas sem sobreposição. Confirmação física do novo ritmo pendente. Retorno ao ritmo anterior por BENCH_N1040_FAST_POLL=0; fonte anterior completa c58b296.


## Primeira interface IHM — 2026-11-23

O usuário confirmou PC10/PC11 e necessidade de instalar o DOPSoft. Implementados UART4/DMA1 Stream1, escravo somente leitura endereço 1, mapa 64 palavras e diagnóstico web. Criado fundo 800×480 e especificação de objetos nativos. Não há projeto DOPSoft compilado nem transferência/validação da IHM física. As três leituras NOVUS no perfil rápido foram confirmadas pelo usuário antes desta mudança; repetir teste integrado após gravar a nova versão.
