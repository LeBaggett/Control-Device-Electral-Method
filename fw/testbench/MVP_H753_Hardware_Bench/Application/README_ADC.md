# Aquisição ADC + DMA — primeiro passo do MVP

Implementação de bancada, versão 0.3.0-adc-dma. Compilação e testes de software não substituem validação elétrica. Nenhum teste físico foi realizado nesta entrega. A entrada ainda não está conectada ao circuito Vf.

## Arquivos e responsabilidades

- `board/pinout.h`: PA6, ADC1 canal positivo 3, entrada single-ended. A troca de pino exige trocar também o canal correspondente.
- `bench_config.h`: taxa inicial de 1.000 amostras/s, referência **nominal** de 3.300 mV e validade temporal de 250 ms.
- `adc_service.h/.c`: inicialização de GPIO, relógio, ADC, calibração interna, TIM6 e DMA1 Stream2; mailbox dos blocos e diagnóstico `g_adc_debug`.
- `adc_stats.h/.c`: estatísticas de 32 amostras e conversão para tensão no pino. Média não equivale a aumento garantido da resolução efetiva.
- `Core/Src/stm32h7xx_it.c`: interrupções ADC e DMA. Os streams 0 e 1 continuam destinados às UARTs.
- `ui/web_files.c`, `ui/index.html`: endpoint `/api/adc` e painel de diagnóstico, atualizado a cada 500 ms.
- `hmi_service.c`: extensão aditiva do mapa para versão 2. Os endereços anteriores permanecem com a mesma função.
- `tests/run_adc_tests.py`: verifica escala, limites, overflow e estatísticas. Os testes HMI/HTTP também cobrem a extensão.

## Estratégia e ordem de inicialização

Após a configuração dos clocks da placa e dos serviços de comunicação:

1. Selecionar CKPER a partir de HSI e divisor assíncrono ADC /4; configurar PA6 analógico, sem pull.
2. Preparar DMA circular e ADC de 16 bits, uma conversão por disparo externo, amostragem de 387,5 ciclos.
3. Executar calibração interna de offset e linearidade do ADC. Isso não calibra o circuito Vf nem a referência da placa.
4. Configurar TIM6 com base de 1 MHz e TRGO de atualização na frequência solicitada. A rotina verifica o clock esperado da bancada (APB1 /2, TIMPRE=0).
5. Armar o DMA **antes** de iniciar o timer.

O timer dá a cadência; o DMA transfere 64 amostras em duas metades. A IRQ copia apenas a metade concluída e publica um contador. O laço principal calcula estatísticas de 32 amostras. A 1 kS/s, cada bloco corresponde a 32 ms. Se a aplicação não consumir um bloco a tempo, o mais recente prevalece e `skipped_blocks` registra os blocos não processados. Este serviço é diagnóstico, não um registrador contínuo sem perdas.

O buffer fica na SRAM AXI, acessível pelo DMA1, alinhado a 32 bytes. D-cache permanece desligado, como na bancada existente; ativá-lo posteriormente exige revisar MPU/cache e coerência DMA.

Falhas de inicialização deixam o ADC inválido e preservam os diagnósticos de fase/status. Falhas de aquisição param timer e DMA no laço principal. Não há repetição automática. Reinicializar a placa permite uma nova tentativa. Nenhuma saída de potência é adicionada.

## Valores e significado

`Ventrada = soma(bloco) × Vref_nominal / (32 × 65535)`.

O valor é tensão em **PA6**, sem ganho/divisor, offset externo ou fator K. A referência real/VDDA não é medida por este código. Resolução configurada de 16 bits não é garantia de precisão de 16 bits.

Live Expressions: `g_adc_debug.phase`, `init_status`, `error`, `valid`, `mean_raw`, `min_raw`, `max_raw`, `last_raw`, `millivolts`, `input_voltage`, `blocks`, `skipped_blocks`, `last_block_ms`.

Fases: 1 clock/GPIO, 2 DMA, 3 ADC/canal, 4 calibração interna, 5 timer, 6 início DMA/timer, 7 aquisição iniciada, 8 erro de aquisição. `init_status` usa HAL_OK=0; `error` contém a máscara HAL ADC. Uma fase inicial com status diferente de zero indica onde a inicialização falhou.

`valid=1` significa bloco recente sem falha de aquisição; não confirma que há DUT conectado, que a tensão é correta ou que o sinal está dentro da faixa metrológica. PA6 desconectado pode flutuar. Os números antigos permanecem disponíveis para diagnóstico quando `valid=0`.

## IHM Delta — novos registradores

Leitura FC03 ou FC04, unsigned 16 bits, endereço STM 1. W40002 passa a indicar mapa 2; W40003 contém 0x0300. Atualizar a arte DOPSoft é uma etapa independente; não foi alterado o arquivo DPA.

| DOPSoft | Conteúdo |
|---|---|
| W40048 | Dados recentes/válidos: 0 ou 1 |
| W40049 | Média bruta ADC: 0…65535 |
| W40050 | Mínimo do bloco |
| W40051 | Máximo do bloco |
| W40052 | Tensão no pino em mV; dividir por 1000 para V |
| W40053 | Taxa de amostragem em Hz |
| W40054 | Contador de blocos, 16 bits inferiores |
| W40055 | Máscara de erro HAL ADC, 16 bits inferiores |
| W40056 | Fase de aquisição |
| W40057 | Referência nominal em mV |
| W40058 | Blocos não processados, 16 bits inferiores |

Offsets 58…63 continuam reservados. Contadores de 16 bits dão a volta; a web e Live Expressions oferecem os contadores de 32 bits. Exibir W40052 condicionado a W40048. Não é necessário alterar firmware para acrescentar telas que leiam estes endereços.

## Validação futura

Hoje não é necessário conectar ou energizar o circuito de aquisição. Quando houver disponibilidade: aplicar tensão DC conhecida e limitada dentro de 0…VDDA, com terra comum; comparar com multímetro, verificar referência real, ruído e contagem de blocos sob tráfego simultâneo Ethernet/NOVUS/IHM. Não aplicar 5 V ao ADC. Nenhuma ligação externa de VREF é exigida neste primeiro passo.

Esta cadência não implementa a captura sincronizada de Vf de um ensaio TSP. A comutação das fontes e os tempos de captura serão implementados após a definição e validação do circuito.
