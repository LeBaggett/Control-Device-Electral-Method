# Calibração do termopar e diagnóstico de Vf

Menu principal > CAL TERMOPAR, separado da CALIBRACAO do diodo/Vf.
A instalação desta atualização não grava ajustes no cartão: isso acontece quando
você escolhe APLICAR/SALVAR no equipamento.

## Comparação com termopar digital: 3 ou 4 temperaturas

1. Coloque as duas pontas próximas, no mesmo meio térmico, isoladas de contatos
   elétricos indesejados, e aguarde equilíbrio. Use aquecimento externo controlado;
   este menu não aciona relé nem SELECT para aquecer, e coloca saídas em estado seguro.
2. Abra CAL TERMOPAR > 3 TEMPERATURAS ou 4 TEMPERATURAS.
3. Em REF, clique para editar; gire para informar a leitura do termopar digital
   em passos de 0,1 °C; clique novamente para sair da edição.
4. Selecione COLETAR PONTO. Aguarde 16 leituras novas do MAX31855 (~4 segundos).
   O programa compara a média bruta com a referência informada. A referência
   digital deve permanecer estável durante a coleta. Se a variação das leituras
   brutas passar de 1 °C, aparece INSTAVEL: REPITA, sem consumir o ponto.
5. Leve ambos a outra temperatura e repita. O campo REF sugere +10 °C depois de
   cada ponto, mas você deve editá-lo para a leitura real do termopar digital.
   Os valores sugeridos não são comandos de um controlador de temperatura.
6. Ao terminar, confira GANHO e R2 e escolha APLICAR/SALVAR. Segurar o botão
   cancela/sai; coleta/ajuste só muda a calibração ativa quando salvar tem sucesso.

Equação: T_corrigida = ganho * T_bruta + offset.
Aceita 3 ou 4 pontos distintos, intervalo bruto total de pelo menos 5 °C,
separação entre pontos >=0,5 °C, R2>=0,98, ganho 0,5..1,5 e offset +/-20 °C.
Esses limites ficam em Application/core/tc_calibration.h e tc_calibration.c.
A referência informada deve estar na faixa TEMP_MIN_C..TEMP_MAX_C do projeto.
Ajuste inválido não é aplicado. Se os pontos não servirem, saia e inicie de novo.
A referência digital também tem sua própria incerteza: este ajuste é comparativo,
não representa certificação metrológica. Não use offset para mascarar sonda
invertida ou tipo de termopar incompatível com a versão do MAX31855.

## Offset / filtro

CAL TERMOPAR > OFFSET/FILTRO:

- OFFSET: coeficiente aditivo em °C, passo de 0,1 °C; preserva o ganho atual.
- FILTRO: alpha entre 0,05 e 1, em passos de 0,05. Menor alpha suaviza mais e atrasa
  a indicação; alpha=1 mostra sem suavização. Padrão 0,25.
- APLICAR/SALVAR grava os valores editados e só então aplica.
- ZERAR AJUSTES abre a confirmação do RESET TOTAL TERMOPAR, incluindo os pontos 3/4 e o backup do SD.

O filtro afeta somente a indicação. NÃO corrige sobre-elevação térmica física,
não faz controle PID e não muda condições de chegada ao alvo. Não foram removidos
picos da entrada de controle para forçar avanço de ensaio.
A temperatura corrigida sem suavização é usada nos alvos, registros e calibração
Vf. A proteção de sobretemperatura considera tanto a bruta como a corrigida: um
offset negativo não pode esconder uma leitura bruta acima do limite.

## SD e calibração Vf

TCAL.TXT armazena ganho, offset, filtro, R2 e número de pontos, com versão e CRC.
TCAL.TMP é verificado antes da substituição e TCAL.BAK preserva o ajuste anterior.
Não há garantia de transação atômica em perda de energia no FAT; teste em bancada.
No boot carrega o arquivo válido, recorrendo ao backup em caso de corrupção.
Sem arquivo válido, usa ganho=1, offset=0 e filtro=0,25. Sem SD ou em erro de
escrita, aparece FALHA CARTAO SD e o ajuste anterior permanece ativo em RAM.

Alterar ganho/offset invalida a calibração Vf anterior em RAM. CALIB.TXT/BAK
passam a incluir os coeficientes do termopar usados: um arquivo com coeficientes
incompatíveis não é carregado como válido. Arquivos antigos sem esses campos são
aceitos apenas com termopar sem correção (ganho=1, offset=0). Refaça a calibração
Vf depois de ajustar o termopar. Alterar só a suavização não exige refazê-la.

## Erro 4 em CAL sem DUT

Erro 4 = falha de medição Vf; erro 6 = armazenamento. Sem DUT/sinal de medição
válido no PC0, a calibração Vf pode parar por erro 4 antes de gravar o ponto.
Isso não impede calibrar o termopar pelo menu novo; o menu não depende de PC0.
A calibração do diodo exige corrente de medição e entrada ADC válidas; mantenha os
limites físicos de PC0. O teste de SD independente existe no projeto de bancada.

A tela agora diferencia FALHA MEDICAO VF. Em Expressions, com debug pausado:

- g_vf_debug.capture_status: 0=OK; 1=ponteiro inválido; 2=ADC não pronto;
  3=pedido pendente; 4=VREFINT antiga (>1s); 5=falha no burst; 6=valor não finito.
- g_vf_debug.burst_status: 0=OK; 1=parâmetros; 2=DWT; 3=início ADC;
  4=timeout durante conversão; 5=leitura zero; 6=saturado; 7=prazo total excedido.
- g_vf_debug.last_vf_raw / last_burst_raw: contagens ADC (0..4095).
- g_vf_debug.completed_samples / requested_samples / elapsed_cycles: andamento
  e duração da última captura. O prazo de 400 us do burst foi preservado.

Termopar em g_hw_debug: tc_raw_c (bruta), tc_corrected_c (corrigida sem filtro),
tc_display_c (bruta filtrada), tc_display_corrected_c (valor mostrado),
tc_gain e tc_offset_c. O filtro em RAM fica em g_hw_tuning.display_filter_alpha.

## Instalar

Terminate Debug > Refresh > Clean > Build > iniciar nova sessão Debug e gravar.
Não precisa regenerar CubeMX. Mantidos encoder -1, SD 1 bit/400 kHz e a liberação
de potência feita pelo usuário; ao iniciar a coleta ou aplicar o ajuste, power_safe.
Projeto de bancada não foi modificado nesta atualização.

Testes de software: navegação em 3/4 pontos, sessão com média e rejeição de
instabilidade, falha de escrita sem aplicar ajuste, recarga, regressão e limites,
CRC/backup, compatibilidade da calibração Vf e proteção térmica com offset.
Regressões e compilação/link ARM passaram; sem validação física pelo assistente.

## Reset total de calibração em 3/4 temperaturas

CAL TERMOPAR > RESET TOTAL TC > CONFIRMAR RESET.
Também acessível por OFFSET/FILTRO > ZERAR AJUSTES, com a mesma confirmação.
Cancela coleta e limpa todas as amostras, referências e candidato da sessão em RAM.
Restaura ganho=1, offset=0, filtro=0,25, R2=1 e pontos salvos=0.
Grava padrões em TCAL.TXT e TCAL.BAK, sem restaurar depois o ajuste antigo pelo
fallback do backup. A mensagem TERMOPAR RESETADO só aparece após sucesso.
Um reset exige SD disponível; em falha de armazenamento, mostra FALHA CARTAO SD.
Se houver interrupção de energia ou erro durante gravação, repita a operação;
FAT não oferece transação atômica. Registros CSV e pinos não são apagados.
A memória do filtro é reinicializada e a próxima leitura atualiza a indicação.
Se ganho/offset mudarem, a calibração Vf é invalidada como nos outros ajustes.
Não executa reset durante ensaio ativo. Ao confirmar, coloca a potência em estado
seguro. A configuração de liberação de potência feita pelo usuário é preservada.
