# board - guia de manutenção

Camada de adaptação STM32/placa: pinagem, HAL, estado seguro, relógio e comunicação. Centraliza os detalhes físicos consumidos pelos drivers.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Pontos de manutenção

- `pinout.h`: pinos, polaridades e controlador OLED; não espalhar números de pinos pelos drivers.
- `runtime_config.h`: debounce/direção/contagens do encoder, espera mecânica do relé (100 ms), ADC e SD.
- `board_init`: HAL → GPIOs seguros → clock → GPIOs gerados → DWT → periféricos → estado seguro.
- `board_force_safe`: SELECT em medição antes de desligar relé. Não inverter a ordem.
- `sofia_io`: caminho rápido do TSP. Não colocar OLED, SD ou atualização VREFINT dentro da janela de captura.
- Diagnóstico: `g_hw_debug`, `g_sd_io_debug` e `g_vf_debug`; usar símbolos completos e ELF correspondente.


## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [board.c](board.c) | Adaptação da placa: inicialização ordenada, estado seguro, intertravamentos de relé/SELECT, relógio de milissegundos e SPI do termopar. Entradas/símbolos para consulta: `board_power_select_measurement`, `board_power_relay_off`, `board_force_safe`, `board_power_select_heating`, `board_power_relay_on`, `board_power_control_enabled`, `board_safe_gpio_early`, `board_init`, `board_is_ready`, `board_millis`, `board_delay_us`, `read_fail`. |
| [board.h](board.h) | Adaptação da placa: inicialização ordenada, estado seguro, intertravamentos de relé/SELECT, relógio de milissegundos e SPI do termopar. Entradas/símbolos para consulta: `board_init`, `board_millis`, `board_delay_us`, `board_power_select_measurement`, `board_power_select_heating`, `board_power_relay_off`, `board_power_relay_on`, `board_is_ready`, `board_power_control_enabled`, `board_force_safe`, `board_max31855_read32`. |
| [cube_glue.c.example](cube_glue.c.example) | Arquivo de suporte cube_glue.c.example; identificar seu consumidor antes de modificar ou remover. |
| [dwt_us.c](dwt_us.c) | Inicializa e usa CYCCNT para atrasos em microssegundos, com verificação de clock e limite de espera. Entradas/símbolos para consulta: `DWT_US_Init`, `DWT_US_Delay`. |
| [dwt_us.h](dwt_us.h) | Inicializa e usa CYCCNT para atrasos em microssegundos, com verificação de clock e limite de espera. Entradas/símbolos para consulta: `DWT_US_Init`, `DWT_US_Delay`. |
| [hw_debug.h](hw_debug.h) | Estruturas de diagnóstico visíveis no depurador; não são comandos de controle. Campos ausentes ou deslocados podem indicar ELF incompatível. Entradas/símbolos para consulta: `board_debug_service`. |
| [INTEGRACAO_CUBEIDE.md](INTEGRACAO_CUBEIDE.md) | Documento específico: integracao cubeide. Conferir a data/contexto e confrontar afirmações históricas com o código entregue. |
| [peripheral_io.c](peripheral_io.c) | Implementação de baixo nível dos periféricos usados pelos drivers, incluindo ADC e captura rápida. Revisar temporização ao alterar o clock. Entradas/símbolos para consulta: `board_oled_controller`, `board_oled_column_offset`, `board_oled_prepare`, `board_oled_write`, `board_encoder_start`, `board_encoder_counter`, `board_encoder_button_pressed`, `board_adc_start`, `board_adc_begin`, `board_adc_cancel`, `board_vrefint_calibration`, `board_adc_sample`. |
| [peripheral_io.h](peripheral_io.h) | Implementação de baixo nível dos periféricos usados pelos drivers, incluindo ADC e captura rápida. Revisar temporização ao alterar o clock. Entradas/símbolos para consulta: `board_oled_controller`, `board_oled_column_offset`, `board_oled_prepare`, `board_oled_write`, `board_encoder_start`, `board_encoder_counter`, `board_encoder_button_pressed`, `board_adc_start`, `board_adc_sample`, `board_adc_begin`, `board_adc_cancel`, `board_vrefint_calibration`. |
| [pinout.h](pinout.h) | Mapa central dos pinos, polaridades, seleção SSD1306/SH1106 e habilitação da potência. Conferir junto ao .ioc e à montagem. |
| [runtime_config.h](runtime_config.h) | Parâmetros de integração: encoder, botão, relé, ADC, SD e limites da calibração por autoaquecimento. |
| [sd_io.c](sd_io.c) | Prepara SDIO/cartão e retém informações de diagnóstico. A implementação do bench pode diferir da versão entregue; não copiar sem revisão. Entradas/símbolos para consulta: `board_sd_is_ready`, `board_sd_prepare`. |
| [sd_io.h](sd_io.h) | Prepara SDIO/cartão e retém informações de diagnóstico. A implementação do bench pode diferir da versão entregue; não copiar sem revisão. Entradas/símbolos para consulta: `sd_le16`, `sd_le32`, `sd_capture_sector`, `board_sd_prepare`, `board_sd_is_ready`. |
| [sofia_io.c](sofia_io.c) | Nome interno legado para a captura rápida do TSP: prepara ADC, comuta SELECT, espera e lê Vf. Não renomear símbolos isoladamente. Entradas/símbolos para consulta: `sofia_io_elapsed_us`, `completed`, `sofia_io_capture`. |
| [sofia_io.h](sofia_io.h) | Nome interno legado para a captura rápida do TSP: prepara ADC, comuta SELECT, espera e lê Vf. Não renomear símbolos isoladamente. Entradas/símbolos para consulta: `sofia_io_capture`, `sofia_io_elapsed_us`. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
