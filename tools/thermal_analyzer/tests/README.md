# tests - guia de manutenção

Thermal Analyzer: aplicação Python/Tkinter para importar CAL/TST/TSP, visualizar medições e gerar relatórios; não controla a potência do dispositivo.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [test_analysis.py](test_analysis.py) | Teste automatizado de analysis. Ler os asserts e os stubs antes de interpretar cobertura; não equivale a ensaio físico. Entradas/símbolos para consulta: `setUp`, `tearDown`, `run_file`, `test_fit_matches_firmware_orientation`, `test_invalid_lines_and_decimal_comma`, `test_header_only_and_constant_calibration`, `test_no_automatic_calibration_pairing_or_fake_missing_tj`, `test_crc_and_tc_metadata`, `test_explicit_model_marks_extrapolation`. |
| [test_sofia.py](test_sofia.py) | Teste automatizado de sofia. Ler os asserts e os stubs antes de interpretar cobertura; não equivale a ensaio físico. Entradas/símbolos para consulta: `run_data`, `test_metadata_timing_and_power`, `test_no_static_result_without_equilibrium`, `test_ignore_unrelated_calibration`, `test_incomplete_file`, `test_current_mismatch_refuses_power`. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
