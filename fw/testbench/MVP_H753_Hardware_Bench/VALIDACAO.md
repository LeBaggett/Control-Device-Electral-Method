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

## Fora desta entrega

RS-485/Modbus, IHM, termopares, ADC/timer TSP, corrente, aquecedor, ajuste de cache/MPU final e ensaio térmico. Nenhuma dessas funções foi simulada como se estivesse operando no dispositivo.


## Etapa 2 — N1040i (2026-11-09)

O usuário confirmou acesso HTTP da etapa anterior após correção da manutenção indevida de cache no RX Ethernet. Isso não valida a serial nova.

Testes novos: `Application/tests/run_n1040_tests.py` (codec e estados reais com transporte simulado, 1 e 3 nós). Regressão: `Application/tests/run_tests.py`. Compilação ARM e integração CubeIDE verificadas nesta entrega; consulte INTEGRACAO_N1040I.md.

| Verificação física | Estado |
|---|---|
| Um N1040i: ID, status e PV iguais ao display | Pendente |
| DE/RE retorna a RX após último stop bit | Pendente |
| Retirar/reconectar serial sem bloquear Ethernet | Pendente |
| Sensor desconectado não resulta em temperatura válida | Pendente |
| Teste contínuo 30 min e duas abas HTTP | Pendente |
| Três indicadores com endereços distintos | Pendente |
