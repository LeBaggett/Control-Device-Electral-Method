# K manual e autoaquecimento na calibraÃƒÂ§ÃƒÂ£o do termopar

AlteraÃƒÂ§ÃƒÂµes somente no projeto oficial. O Driver_Hardware_Bench continua com controle manual.

## Ensaio com K manual
MENU > ENSAIO K MANUAL. K ÃƒÂ© o coeficiente **assinado dT/dVf em Ã‚Â°C/V**, por exemplo -500; nÃƒÂ£o informar 2 mV/Ã‚Â°C nesse campo.
Clique no valor, ajuste com o encoder e clique novamente para confirmar. PASSO muda a resoluÃƒÂ§ÃƒÂ£o de ediÃƒÂ§ÃƒÂ£o. CONFIG ENSAIO abre inÃƒÂ­cio/fim/passo e a confirmaÃƒÂ§ÃƒÂ£o final.
Antes de INICIAR, deixe DUT e termopar em equilÃƒÂ­brio tÃƒÂ©rmico, sem aquecimento prÃƒÂ©vio. O programa captura Vf de referÃƒÂªncia em mediÃƒÂ§ÃƒÂ£o (SELECT alto), com relÃƒÂ© habilitado e atraso DWT de 30 Ã‚Âµs, antes de selecionar aquecimento. Usa **Tj = Tref + K Ãƒâ€” (Vf Ã¢Ë†â€™ Vfref)**. A temperatura do termopar continua sendo a referÃƒÂªncia dos alvos.
O K ÃƒÂ© temporÃƒÂ¡rio em RAM e precisa ser revisado apÃƒÂ³s reiniciar. NÃƒÂ£o substitui CALIB.TXT nem altera a calibraÃƒÂ§ÃƒÂ£o de Vf jÃƒÂ¡ salva. O CSV registra K e o par Tref/Vfref em comentÃƒÂ¡rio #mode=MANUAL_K; as colunas de dados continuam iguais.
SD, ADC ou potÃƒÂªncia indisponÃƒÂ­veis impedem o inÃƒÂ­cio. Sobreaquecimento por Tj calculada tambÃƒÂ©m encerra o ensaio.

## Autoaquecer — confirmação direta do alvo digital
Na mesma opção MENU > CAL TERMOPAR > AUTOAQUECER, sem PID/PI/PD e sem campo REF.
1. Posicione as duas sondas juntas e confirme SD OK.
2. Escolha 3/4 pontos, início e passo (ex.: 30, 40, 50 e 60 °C).
3. INICIAR AQUECIMENTO habilita medição, liga relé, espera 100 ms e seleciona aquecimento contínuo. O firmware não regula a temperatura automaticamente.
4. Observe o TERMOPAR DIGITAL. Quando indicar o alvo exibido, clique em CONFIRMAR ALVO, já selecionado. A confirmação significa que o instrumento digital está naquele alvo; não há leitura eletrônica desse instrumento.
5. O clique desliga a potência e associa a última leitura bruta válida do MAX31855 ao alvo presetado. A leitura precisa ter no máximo 300 ms; a aplicação atualiza o sensor a cada 250 ms. Não há média posterior de quatro segundos, espera de estabilidade nem edição de referência.
6. O aquecimento fica desligado entre pontos. Clique em PROXIMO ALVO quando estiver pronto; repita a confirmação no instrumento digital. O alvo só avança por esse comando.
7. Depois do último ponto, APLICAR/SALVAR grava ganho/offset em TCAL.TXT com backup/CRC. Mudanças no ganho/offset invalidam a calibração Vf anterior. O ajuste fino posterior continua em OFFSET/FILTRO.
Segurar o encoder cancela e desliga. Permanecem proteções de sensor, dado velho, temperatura local excessiva e tempo limite. A temperatura pode continuar subindo por inércia após desligar. Confirme no instante correto; a resolução temporal da leitura local e a resposta das duas sondas limitam a precisão. Ajuste de offset posterior não corrige diferenças de atraso entre sondas.
As opções manuais 3 TEMPERATURAS / 4 TEMPERATURAS fora do AUTOAQUECER mantêm seu fluxo anterior com referência digitada e média de leituras.

## Ajustes e diagnÃƒÂ³stico
Application/board/pinout.h: mapeamento central, SELECT PD12 com pull-down e relÃƒÂ© PD14. A polaridade e a sequÃƒÂªncia de seguranÃƒÂ§a permanecem as existentes.
Application/board/runtime_config.h: TC_HEAT_CONFIRM_MAX_AGE_MS=300, limite de alvo 80 °C, passo mínimo 5 °C, sensor máximo 1000 ms em aquecimento, timeout 10 min por ponto e limite de segurança 10 °C acima do alvo local. Não há ganhos de controlador.
Live Expressions: g_tc_heat_debug (active, heating, waiting_next, target_c, temperature_c, point, points, error). error: 1 configuração; 2 sensor; 3 temperatura; 4 timeout; 5 potência bloqueada.
No CubeIDE: Refresh > Clean > Build; grave o novo ELF gerado pelo seu projeto e use os sÃƒÂ­mbolos dele no debug.

## VerificaÃƒÂ§ÃƒÂ£o
Testes automatizados simulam saÃƒÂ­das, referÃƒÂªncias, falhas e salvamento. A compilaÃƒÂ§ÃƒÂ£o ARM usa os fontes HAL/FatFs do projeto. Isso nÃƒÂ£o constitui validaÃƒÂ§ÃƒÂ£o fÃƒÂ­sica nem comprova estabilidade tÃƒÂ©rmica da PCB.

