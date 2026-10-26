# core - guia de manutenção

Projeto manual de validação de drivers e comunicação. O estado local inclui diagnóstico SD somente de leitura e pode divergir da versão oficial entregue.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [format.c](format.c) | Formatação numérica usada no OLED e nos registros, evitando depender da impressão float da biblioteca reduzida. Entradas/símbolos para consulta: `format_decimal`, `thermal_crc32`. |
| [format.h](format.h) | Formatação numérica usada no OLED e nos registros, evitando depender da impressão float da biblioteca reduzida. Entradas/símbolos para consulta: `format_decimal`, `thermal_crc32`. |
| [linfit.h](linfit.h) | Regressão linear e indicadores do ajuste. O modelo elétrico usa temperatura como função de Vf. Entradas/símbolos para consulta: `linfit`, `cal_tj`. |
| [vf_math.c](vf_math.c) | Conversão de contagens ADC em volts e estimativa de VDDA por VREFINT. Conferir unidade, resolução e calibração de referência. Entradas/símbolos para consulta: `vf_from_counts`, `vdda_from_vrefint`. |
| [vf_math.h](vf_math.h) | Conversão de contagens ADC em volts e estimativa de VDDA por VREFINT. Conferir unidade, resolução e calibração de referência. Entradas/símbolos para consulta: `vf_from_counts`, `vdda_from_vrefint`. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
