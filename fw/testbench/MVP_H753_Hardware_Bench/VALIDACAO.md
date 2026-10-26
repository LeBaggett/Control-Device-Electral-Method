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
