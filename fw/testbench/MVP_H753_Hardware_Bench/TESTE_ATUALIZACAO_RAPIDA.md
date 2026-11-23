# Experimento de atualização rápida — 2026-11-23

Ponto de retorno anterior: commit c58b296, versão 0.2.4-three-n1040i. Versão de teste: 0.2.5-poll-test. Não foi feita validação física pelo agente.

## O que muda e por quê

- `BENCH_N1040_FAST_POLL=1`: intervalo mínimo entre inícios de consultas reduzido de 250 para 25 ms. Continuam seis pedidos por rodada: estados e PV dos endereços 247, 246, 245.
- O protocolo continua serial, com uma transação pendente por vez. Nenhuma pergunta é sobreposta à resposta anterior. Se a resposta demorar mais de 25 ms, ela é aguardada até o timeout existente de 400 ms.
- `/api/n1040` passa a informar poll_profile, poll_ms e web_refresh_ms; o navegador usa esse último campo para programar a próxima atualização. Isso permite retornar aos dois ritmos alterando uma única flag no firmware.
- O painel de temperaturas espera 250 ms após terminar cada consulta HTTP; o diagnóstico geral permanece a cada segundo. Não há fetches sobrepostos dentro de cada laço da página.
- Baud 115200/8N1, endereços, UART/DMA, silêncio RTU, validade de dados, corrente e configuração de potência não mudam. UART4/IHM continua pendente da confirmação dos pinos e do projeto serial.

## Resultado esperado e limites

Com respostas suficientemente rápidas, seis pedidos iniciados a cada 25 ms dão um ciclo de aproximadamente 150 ms para cada indicador. O intervalo entre leituras dos três canais ainda existe, e as conversões internas não são sincronizadas. Essa é uma estimativa de comunicação, não prova de taxa física do sensor. Falhas/indicadores ausentes ainda podem acrescentar 400 ms por pedido não respondido.

O navegador mostra um snapshot dos últimos valores por canal; ele não afirma que foram adquiridos ao mesmo tempo. Para o ensaio futuro, preservar timestamp/idade e o estado válido de cada amostra.

## Como executar

1. Anotar os contadores atuais como referência e manter a mesma montagem, alimentação e configurações dos indicadores.
2. Refresh, Clean e Build no projeto da bancada; gravar e executar com F8.
3. Abrir a página com Ctrl+F5. Confirmar versão 0.2.5-poll-test, perfil fast-test, poll_ms=25, web_refresh_ms=250 e três canais configurados.
4. Observar por 10–30 minutos os três cartões e o JSON. responses/rx_bytes devem avançar; anotar novos timeouts, CRC, protocol_errors, uart_errors e restarts. Os contadores são cumulativos: comparar incrementos no período.
5. Verificar rtt_ms em cada nó. Se o tempo de resposta se aproxima/excede 25 ms, o ciclo real será maior que a estimativa. Não alterar o baud dos indicadores junto com este teste.
6. Confirmar que a página e os três valores continuam atualizando. Repetir com duas abas apenas depois de confirmar uma aba estável.

## Retorno simples

Em Application/bench_config.h, alterar apenas `#define BENCH_N1040_FAST_POLL 1U` para `0U`, recompilar e gravar. A API passa a informar perfil previous, consultas 250 ms e tela 1000 ms. Não é necessário desfazer commits nem apagar arquivos.

Se for necessário recuperar exatamente a fonte anterior inteira, ela continua no histórico do commit c58b296. Não executar reset/clean destrutivo sobre trabalho local para isso.

## Verificação no computador

Os testes exercitam um e três nós nos dois perfis, respeitam deadlines mínimos, verificam que resposta lenta não gera uma segunda requisição concorrente e mantêm a cobertura de CRC, estados, timeout, nó ausente e dados antigos. A página passa por validação sintática JavaScript e regressão HTTP/LwIP; o firmware é compilado e ligado para Cortex-M7.
