# Método implementado e decisões de integração

Os parâmetros numéricos vêm do planejamento enviado pelo usuário.
A decisão mais recente MAX31855 prevalece sobre o PT100/MAX31865 antigo.
A implementação preserva as APIs v5 quando possível e estende os comandos
de calibração para também receber início/fim/passo, sem refazer a HAL na UI.

## Calibração

Defaults da UI: 25..85 °C, passo 10. Limites: 0..150 °C, máximo 200 pontos.
Exige pelo menos dois pontos antes de abrir arquivo. Abre CAL_nnn.CSV,
seleciona medição e tenta relé ON. Enquanto o board estiver bloqueado,
fecha a sessão e retorna JOB_ERROR_POWER_LOCKED.

Com o hardware simulado autorizado nos testes, aguarda ±0,5 °C do alvo
e mantém essa faixa por 180 s. Sair da faixa reinicia o soak. Faz média
cooperativa de 64 conversões Vf usando VDDA/VREFINT, grava a linha e avança
o alvo. Timeout de 3600 s por alvo inclui soak e coleta.

No fim ajusta T = m*Vf + T0 em double e calcula R². Variância nula de Vf ou
T, NaN/Inf e menos de dois pontos são recusados. Coeficientes válidos são
persistidos antes de atualizar o estado. Menos de cinco pontos é mostrado
como aviso. Não foi criado um limiar arbitrário de R²: o plano não o define.

## Teste

Defaults da UI: 25..100 °C, passo 5. REQUIRE_CAL=0 segue o protótipo;
REQUIRE_CAL=1 recusa sem calibração antes de qualquer escrita/acionamento.
O início abre TST_nnn.CSV, SELECT=medição e só então tenta relé/aquecimento.
O board real desta versão sempre recusa essas operações.

Na simulação, quando temperatura do termopar ≥ alvo: SELECT em medição,
espera 30 us, captura oito conversões ADC, calcula Tj e grava o ponto.
Não há I2C/SD entre SELECT e captura. O retorno a heating ocorre somente
após a gravação do ponto, se houver outro alvo; no último, power_safe.
Essa escolha mantém medição durante SD síncrono e altera o tempo entre
os pontos em relação ao plano que retomava aquecimento antes de escrever.
A forma de onda e os tempos ainda precisam ser medidos e revisados no alvo.

Foi resolvida a contradição do texto do plano sobre esperar antes/depois
da chave: foi adotada a sequência do BPMN e do teste detalhado, **SELECT
primeiro, depois 30 us**, coerente com remover o transiente da comutação.

Timeout de 600 s/alvo; limite duro 160 °C. Ao primeiro erro de leitura do
sensor, potência é retirada imediatamente. A mensagem final de erro aparece
após três falhas distintas consecutivas, contadas por sensor_sequence,
não pelo número de voltas do loop. Recuperação antes do limite reinicia a
espera do alvo/soak. SD/ADC/timeout/sobretemperatura/aborto saem seguros.

## Arquivos

- CAL_nnn.CSV: `idx,t_alvo_c,t_rtd_c,vf_v`.
- TST_nnn.CSV: `idx,t_alvo_c,t_rtd_c,vf_v,tj_c,t_s`.
- `t_rtd_c` é mantido por compatibilidade do formato, mas agora representa
  o termopar MAX31855. `tj_c` sem calibração é `NaN`.
- Índice a partir de zero; alvo/temperatura com duas casas, Vf com cinco,
  Tj com duas, tempo em segundos com três. Nomes de 001 a 999, CREATE_NEW.
- CALIB.TXT: cabeçalho `m,t0,r2,n`, linha de valores e rodapé `#crc=...`.
  Os quatro campos planejados foram preservados; CRC/backup são extensões.
  m é persistido com três casas, T0 com quatro e R² com seis; validar perda
  de precisão para o experimento. Valores grandes que não cabem na
  formatação são recusados. Arquivos antigos sem CRC não são aceitos.
- Primeiro escreve/sincroniza/verifica CALIB.TMP; mantém calibração anterior
  válida em CALIB.BAK e renomeia. Se o principal estiver corrompido, o
  loader tenta BAK. Rename/FAT não fornece garantia de transação atômica
  durante queda de energia; ensaiar falhas físicas e guardar cópias externas.

Não há identificação automática do DUT ou da corrente no CALIB.TXT. Só
reutilizar a calibração com a mesma cadeia de medição e condições elétricas.
