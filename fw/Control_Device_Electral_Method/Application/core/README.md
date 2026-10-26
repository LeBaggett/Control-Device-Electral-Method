# core - guia de manutenção

Estado e algoritmos das sessões: calibração elétrica, ensaio por alvos, K manual, termopar e TSP. Manter unidades e sequência de potência explícitas.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Modelos e invariantes

- `job`: calibração elétrica externa e ensaio por alvos; captura ao alcançar o alvo, sem estabilização térmica automática.
- `sofia`: nome interno legado do ENSAIO TSP; não confundir com o helper matemático `thermal_tsp`.
- TSP: `Tj = m * Vf[V] + b`; potência `P = 0.758 * Vh[V]`. Vf é medida sob corrente nominal 2 mA, Vh sob 758 mA.
- K manual: `Tj = Tref + K * (Vf - Vfref)`, com K assinado em °C/V.
- `tc_calibration`: temperatura corrigida por ganho e offset; ajustes de termopar e reta elétrica são calibrações diferentes.
- Toda saída de sessão deve cancelar aquisição e levar a potência ao estado seguro antes de operações de fechamento no SD.


## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [calibration.c](calibration.c) | Estado e aplicação da calibração elétrica do DUT. Ganho/offset do termopar são tratados separadamente. Entradas/símbolos para consulta: `calibration_init`, `calibration_set`. |
| [calibration.h](calibration.h) | Estado e aplicação da calibração elétrica do DUT. Ganho/offset do termopar são tratados separadamente. Entradas/símbolos para consulta: `calibration_init`, `calibration_set`. |
| [device.h](device.h) | Estruturas do estado compartilhado entre orquestração, sessão e interface. Alterar campos exige conferir produtores e consumidores. |
| [format.c](format.c) | Formatação numérica usada no OLED e nos registros, evitando depender da impressão float da biblioteca reduzida. Entradas/símbolos para consulta: `format_decimal`, `thermal_crc32`. |
| [format.h](format.h) | Formatação numérica usada no OLED e nos registros, evitando depender da impressão float da biblioteca reduzida. Entradas/símbolos para consulta: `format_decimal`, `thermal_crc32`. |
| [job.c](job.c) | Máquina de execução da calibração elétrica e dos ensaios por alvos, incluindo K manual, falhas, aborto e finalização segura. Entradas/símbolos para consulta: `job_error`, `job_kind`, `job_init`, `stop`, `fail`, `start`, `job_start_calibration_params`, `job_start_calibration`, `job_start_test`, `job_start_test_manual_k`, `sampled_tj`, `job_abort`. |
| [job.h](job.h) | Máquina de execução da calibração elétrica e dos ensaios por alvos, incluindo K manual, falhas, aborto e finalização segura. Entradas/símbolos para consulta: `job_start_test_manual_k`, `job_init`, `job_start_calibration`, `job_start_calibration_params`, `job_start_test`, `job_abort`, `job_step`, `job_error`, `job_kind`. |
| [linfit.c](linfit.c) | Regressão linear e indicadores do ajuste. O modelo elétrico usa temperatura como função de Vf. Entradas/símbolos para consulta: `linfit`, `cal_tj`. |
| [linfit.h](linfit.h) | Regressão linear e indicadores do ajuste. O modelo elétrico usa temperatura como função de Vf. Entradas/símbolos para consulta: `linfit`, `cal_tj`. |
| [sofia.c](sofia.c) | Nome interno legado do controlador ENSAIO TSP: aquecimento por tempo, corte, captura quente, resfriamento e gravação posterior. Entradas/símbolos para consulta: `stop`, `fail`, `sofia_init`, `meta`, `sofia_start`, `sofia_measure_now`, `sofia_abort`, `flush`, `sofia_step`. |
| [sofia.h](sofia.h) | Nome interno legado do controlador ENSAIO TSP: aquecimento por tempo, corte, captura quente, resfriamento e gravação posterior. Entradas/símbolos para consulta: `sofia_init`, `sofia_start`, `sofia_step`, `sofia_measure_now`, `sofia_abort`. |
| [targets.c](targets.c) | Valida início/fim/passo e gera os alvos térmicos respeitando os limites do projeto. Entradas/símbolos para consulta: `targets_count`, `targets_at`. |
| [targets.h](targets.h) | Valida início/fim/passo e gera os alvos térmicos respeitando os limites do projeto. Entradas/símbolos para consulta: `targets_count`, `targets_at`. |
| [tc_calibration.c](tc_calibration.c) | Ajuste afim do termopar, validação de perfis e regressão com 3/4 pontos. Mudanças repercutem na validade da calibração elétrica. Entradas/símbolos para consulta: `tc_calibration_init`, `tc_calibration_valid`, `tc_calibration_set`, `tc_calibration_offset`, `tc_calibration_apply`, `tc_calibration_fit`. |
| [tc_calibration.h](tc_calibration.h) | Ajuste afim do termopar, validação de perfis e regressão com 3/4 pontos. Mudanças repercutem na validade da calibração elétrica. Entradas/símbolos para consulta: `tc_calibration_init`, `tc_calibration_valid`, `tc_calibration_set`, `tc_calibration_offset`, `tc_calibration_apply`, `tc_calibration_fit`. |
| [tc_heater.c](tc_heater.c) | Sessão AUTOAQUECER do termopar: confirmação manual do alvo, desligamento entre pontos e guardas de sensor/tempo/temperatura. Não é um PID. Entradas/símbolos para consulta: `tc_heater_stop`, `fail`, `sensor_ok`, `energize`, `tc_heater_start`, `tc_heater_service`, `tc_heater_next`, `tc_heater_advance`. |
| [tc_heater.h](tc_heater.h) | Sessão AUTOAQUECER do termopar: confirmação manual do alvo, desligamento entre pontos e guardas de sensor/tempo/temperatura. Não é um PID. Entradas/símbolos para consulta: `tc_heater_start`, `tc_heater_service`, `tc_heater_next`, `tc_heater_advance`, `tc_heater_stop`. |
| [thermal_tsp.c](thermal_tsp.c) | Utilitários matemáticos do coeficiente assinado e da conversão ancorada T = Tref + K·(Vf − Vfref), usada pelo ensaio K manual. Entradas/símbolos para consulta: `tsp_slope_valid`, `tsp_temperature`. |
| [thermal_tsp.h](thermal_tsp.h) | Utilitários matemáticos do coeficiente assinado e da conversão ancorada T = Tref + K·(Vf − Vfref), usada pelo ensaio K manual. Entradas/símbolos para consulta: `tsp_slope_valid`, `tsp_temperature`. |
| [vf_math.c](vf_math.c) | Conversão de contagens ADC em volts e estimativa de VDDA por VREFINT. Conferir unidade, resolução e calibração de referência. Entradas/símbolos para consulta: `vf_from_counts`, `vdda_from_vrefint`. |
| [vf_math.h](vf_math.h) | Conversão de contagens ADC em volts e estimativa de VDDA por VREFINT. Conferir unidade, resolução e calibração de referência. Entradas/símbolos para consulta: `vf_from_counts`, `vdda_from_vrefint`. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
