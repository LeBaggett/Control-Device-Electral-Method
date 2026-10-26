# Ensaio TSP: degrau de potência e TSP

Este é um novo ensaio, no MENU > ENSAIO TSP, com sequência independente dos ensaios por alvos anteriores. Utiliza os mesmos drivers de GPIO, SD, ADC e MAX31855, preservando pinout.h, inicialização segura e as correções anteriores. Driver_Hardware_Bench não é alterado.

## Configuração física confirmada
- Corrente fixa de medição: 2 mA.
- Corrente fixa de aquecimento: 758 mA.
- MAX31855 instalado na caixa do DUT (Tcase), não no ambiente.
- SELECT PD12: medição alto / aquecimento baixo. RELAY PD14 ativo alto.
As correntes vêm do circuito físico, não são programadas nem editadas pelo firmware. O cálculo usa 0,758 A. ADC PC0 precisa ler a tensão nos terminais do DUT tanto em aquecimento quanto em medição. Essa condição deve ser conferida na bancada; não se mede corrente ou queda nos cabos pelo firmware.

## Operar
1. Confirme SD OK, Vf coerente e termopar válido, com o DUT inicialmente em equilíbrio.
2. MENU > ENSAIO TSP. Revise TEMPO s, M C/V, B C, ATRASO us e MAX CASE C. A tela rola ao girar o encoder. Clique para editar e novamente para confirmar.
3. A equação é Tj = M × Vf[V] + B. Se houver calibração válida, M/B são carregados dela como valores iniciais para revisão. Caso contrário, são propostos os coeficientes fornecidos pelo usuário: M=-467 °C/V e B=279,36 °C. Isso não comprova calibração do DUT atual; confira sua reta obtida sob 2 mA. Não confundir M com dV/dT em mV/°C.
4. O tempo começa em 1 s para uma primeira verificação; pode ser ajustado de 1 a 600 s. MAX CASE padrão 85 °C. ATRASO padrão 20 µs, ajustável de 5 a 1000 µs para permitir verificar a acomodação do circuito. O intervalo 5–20 µs do roteiro não é garantia de acomodação real.
5. REVISAR/INICIAR > INICIAR DEGRAU: SELECT em medição, relé ligado, espera mecânica de 100 ms, leitura inicial sob 2 mA e início de aquecimento contínuo sob 758 mA. O relé não é usado como chave rápida.
6. Ao fim do tempo, ou ao clicar durante TSP HEAT, mede a tensão sob 758 mA, prepara o ADC ainda em aquecimento, descarta essa conversão e comuta o SELECT para medição. Aguarda o atraso solicitado e dispara uma nova conversão de Vf sob 2 mA. São registrados os instantes de início/fim dessa conversão em relação à fronteira de software do SELECT.
7. Mantém somente medição durante mais 5 s e registra pontos de resfriamento. A malha principal solicita 1/2/5/10/20/50/100/200/500 ms e 1/2/5 s, mas grava os instantes realmente observados, sem inventar amostras perdidas. A primeira leitura quente tem aquisição específica; as seguintes são cooperativas, não DMA contínuo.
8. Desliga a potência e só então grava as amostras em TST_XXX.CSV, com #mode=TSP_STEP. Não escreve SD dentro da janela de captura/resfriamento. Dados em RAM podem ser perdidos se a energia cair antes do fechamento do arquivo.
Segurar o encoder aborta e desliga. Falhas de SD/ADC/termopar, limite de caixa e limite geral de Tj/temperatura bruta encerram o ensaio. Tj só pode ser inferida sob a corrente de medição: durante aquecimento, proteção térmica usa o termopar na caixa, sujeito ao atraso térmico real.

## Relatório
Abra tools/thermal_analyzer/INICIAR.bat, importe o TST_XXX.CSV e mantenha Usar tj_c gravada no SD. O arquivo é reconhecido como TSP automaticamente.
O relatório inclui M/B, as duas correntes fixas, Vf inicial/quente, tensão sob aquecimento, potência P=0,758×Vh, duração programada/observada, atraso pedido/observado, idade da leitura da caixa, instantes de resfriamento e origem/hash do CSV. Corrente/potência não são campos para substituir as correntes físicas deste ensaio.
A temperatura ambiente deve ser informada separadamente no aplicativo, se desejada Rθj-a. A caixa medida pelo MAX31855 não substitui Ta. Marque regime permanente SOMENTE se houver verificado estabilidade antes do corte; o tempo programado não comprova equilíbrio. Nessa condição calcula Rθj-a estimada no ponto quente. A razão (Tj-Tcase)/P é identificada como aparente, específica do arranjo. Não há Tb, portanto não há thetaJB.
Não são sintetizados modelo RC, função de estrutura, constante de tempo ou extrapolação para t=0. A potência residual da corrente de medição não é compensada na fórmula aproximada P=Ih×Vh.

## Código e verificação
Application/core/sofia.c/.h: processo independente; correntes físicas fixas IH_A=0.758 e IM_A=0.002.
Application/board/sofia_io.c/.h: preparação ADC, comutação e marcadores DWT.
Live Expressions: g_sofia_debug (phase 1 aquecendo, 2 resfriando, 3 completo; error com os códigos de job; samples; heating_v; heating_w; hot_vf; tj_hot; first_us; end_us).
No CubeIDE: Refresh > Clean > Build. Não há mudança de pinagem/.ioc para o novo ensaio. A compilação ARM e os testes simulados não são validação elétrica; a acomodação e a janela real devem ser verificadas com osciloscópio antes de interpretar a primeira leitura como temperatura no instante do corte.

