# Captura de Vf ao detectar o alvo

Aplica-se ao projeto principal, por solicitação do usuário.

CAL e HEAT agora usam o mesmo disparo: primeira leitura nova, válida, cuja
Temperatura bruta seja maior ou igual ao alvo. Nessa mesma chamada de job_step:

1. Selecionar medição.
2. Esperar 30 us por DWT.
3. Capturar Vf em burst ADC (64 amostras em CAL, 8 em HEAT).
4. Manter medição enquanto salva o ponto no SD.
5. Avançar o alvo; em HEAT, voltar a aquecer após gravar com sucesso.

Não há espera de 180 segundos, banda de tolerância nem exigência de permanência
no alvo. CAL permanece em medição, com aquecimento externo como antes.
Os antigos CAL_SOAK_S e CAL_TOL_C ficaram definidos por compatibilidade, mas não
participam mais desse protocolo. CAL_MEAS_DELAY_US e TEST_MEAS_DELAY_US estão em
Application/config.h, ambos com padrão 30U.

Os 30 us são a espera após SELECT antes da captura, não a duração total do burst
nem uma garantia de amostrar 30 us depois do cruzamento físico da temperatura.
O MAX31855 continua sendo consultado a cada 250 ms; a detecção depende de uma
amostra nova e usa a leitura bruta, não o valor suavizado exibido no OLED.
Não ocorre trabalho de OLED/FatFs no trecho SELECT -> espera -> ADC.

A qualidade da calibração depende dos pontos efetivamente medidos. Sem espera de
equilíbrio, os resultados podem diferir do protocolo anterior. Pontos sem variação
suficiente de Vf continuam sendo rejeitados pela regressão, sem salvar calibração
inválida. Sobretensão ADC/limites do hardware não são corrigidos por software.

Mantidos: falha segura, limite térmico, timeout de alvo, proteção contra erros de
sensor/ADC/SD, arquivos novos sem sobrescrever registros anteriores, encoder -1,
SD 1 bit / 400 kHz e a liberação de potência feita pelo usuário.

Encerre Debug, Refresh, Clean, Build e grave novamente. Testes de software do
protocolo, passos pequenos, falhas, regressão e SD passaram; compilação/link ARM
passaram. Não houve medição física do intervalo SELECT/ADC nesta atualização.
