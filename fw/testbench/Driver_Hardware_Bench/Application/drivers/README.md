# drivers - guia de manutenção

Projeto manual de validação de drivers e comunicação. O estado local inclui diagnóstico SD somente de leitura e pode divergir da versão oficial entregue.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [encoder.c](encoder.c) | Converte contador TIM2 e botão em eventos de giro, clique e pressão longa; inclui filtros e tratamento de wrap. Entradas/símbolos para consulta: `encoder_init`, `encoder_poll`, `encoder_get_event`. |
| [encoder.h](encoder.h) | Converte contador TIM2 e botão em eventos de giro, clique e pressão longa; inclui filtros e tratamento de wrap. Entradas/símbolos para consulta: `encoder_init`, `encoder_get_event`. |
| [hw_debug.c](hw_debug.c) | Estruturas de diagnóstico visíveis no depurador; não são comandos de controle. Campos ausentes ou deslocados podem indicar ELF incompatível. |
| [max31855.c](max31855.c) | Decodificação do frame SPI de 32 bits, temperatura assinada e falhas de termopar. Entradas/símbolos para consulta: `max31855_init`, `max31855_decode`, `max31855_read`. |
| [max31855.h](max31855.h) | Decodificação do frame SPI de 32 bits, temperatura assinada e falhas de termopar. Entradas/símbolos para consulta: `max31855_init`, `max31855_decode`, `max31855_read`. |
| [oled.c](oled.c) | Framebuffer e transporte I2C para OLED; perfis SSD1306/SH1106, desenho de texto e envio incremental. Entradas/símbolos para consulta: `oled_try_init`, `oled_clear`, `oled_ready`, `oled_busy`, `oled_pixel`, `oled_rect`, `oled_text`, `oled_bitmap`, `oled_text_big`, `oled_xbm`, `oled_present`, `oled_service`. |
| [oled.h](oled.h) | Framebuffer e transporte I2C para OLED; perfis SSD1306/SH1106, desenho de texto e envio incremental. Entradas/símbolos para consulta: `oled_init`, `oled_ready`, `oled_clear`, `oled_pixel`, `oled_rect`, `oled_text`, `oled_text_big`, `oled_xbm`, `oled_bitmap`, `oled_present`, `oled_service`, `oled_recover_service`. |
| [power.c](power.c) | API de medição/aquecimento e retorno seguro; delega ao board os GPIOs e intertravamentos. Entradas/símbolos para consulta: `power_safe`, `power_init`, `power_enable_measurement`, `power_relay_on`, `power_select_heating`, `power_is_safe`. |
| [power.h](power.h) | API de medição/aquecimento e retorno seguro; delega ao board os GPIOs e intertravamentos. Entradas/símbolos para consulta: `power_init`, `power_safe`, `power_enable_measurement`, `power_select_heating`, `power_relay_on`, `power_is_safe`. |
| [rtd.c](rtd.c) | Adaptador de compatibilidade do nome antigo para MAX31855/termopar; no principal aplica a calibração do termopar. Entradas/símbolos para consulta: `rtd_init`, `rtd_read_temperature`. |
| [rtd.h](rtd.h) | Adaptador de compatibilidade do nome antigo para MAX31855/termopar; no principal aplica a calibração do termopar. Entradas/símbolos para consulta: `rtd_init`, `rtd_read_temperature`. |
| [sdlog.c](sdlog.c) | Montagem FatFs, CSV sem sobrescrita, escrita/sincronização e persistência de ajustes com CRC e backup. Entradas/símbolos para consulta: `sdlog_ready`, `write_exact`, `storage_error`, `sdlog_init`, `sdlog_open_run`, `sdlog_line`, `sdlog_close`, `valid_cal`, `parse_cal`, `load_file`, `sdlog_load_calibration`, `sdlog_save_calibration`. |
| [sdlog.h](sdlog.h) | Montagem FatFs, CSV sem sobrescrita, escrita/sincronização e persistência de ajustes com CRC e backup. Entradas/símbolos para consulta: `sdlog_init`, `sdlog_ready`, `sdlog_open_run`, `sdlog_line`, `sdlog_close`, `sdlog_save_calibration`, `sdlog_load_calibration`. |
| [sdprobe.c](sdprobe.c) | Teste de arquivo de bancada. Pode criar e reler arquivo quando chamado; conferir se o menu atual está em diagnóstico somente leitura. Entradas/símbolos para consulta: `sdprobe_roundtrip`. |
| [sdprobe.h](sdprobe.h) | Teste de arquivo de bancada. Pode criar e reler arquivo quando chamado; conferir se o menu atual está em diagnóstico somente leitura. Entradas/símbolos para consulta: `sdprobe_roundtrip`. |
| [vf_adc.c](vf_adc.c) | Aquisição de Vf: serviço de fundo, referência VREFINT, captura sob demanda e diagnóstico de timeout/zero/saturação. Entradas/símbolos para consulta: `vf_adc_raw_count`, `convert`, `vf_adc_init`, `vf_adc_ready`, `vf_adc_cancel_samples`, `vf_adc_vdda`, `vf_adc_vdda_fallback`, `vf_adc_begin_samples`, `vf_adc_service`, `vf_adc_take_samples`, `vf_adc_read_voltage`, `vf_adc_capture`. |
| [vf_adc.h](vf_adc.h) | Aquisição de Vf: serviço de fundo, referência VREFINT, captura sob demanda e diagnóstico de timeout/zero/saturação. Entradas/símbolos para consulta: `vf_adc_init`, `vf_adc_raw_count`, `vf_adc_read_voltage`, `vf_adc_service`, `vf_adc_ready`, `vf_adc_begin_samples`, `vf_adc_take_samples`, `vf_adc_capture`, `vf_adc_vdda`, `vf_adc_vdda_fallback`, `vf_adc_cancel_samples`. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
