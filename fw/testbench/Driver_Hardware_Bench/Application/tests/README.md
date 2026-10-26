# tests - guia de manutenção

Projeto manual de validação de drivers e comunicação. O estado local inclui diagnóstico SD somente de leitura e pode divergir da versão oficial entregue.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Subpastas

| Pasta | Responsabilidade |
|---|---|
| [stubs](stubs/README.md) | Projeto manual de validação de drivers e comunicação. O estado local inclui diagnóstico SD somente de leitura e pode divergir da versão oficial entregue. |

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [bench_test.c](bench_test.c) | Teste automatizado de bench. Ler os asserts e os stubs antes de interpretar cobertura; não equivale a ensaio físico. Entradas/símbolos para consulta: `record`, `board_is_ready`, `board_power_control_enabled`, `board_millis`, `board_force_safe`, `board_power_select_measurement`, `board_power_relay_off`, `board_power_relay_on`, `board_power_select_heating`, `board_bench_led_init`, `board_bench_led_write`, `board_bench_core_clock`. |
| [fatfs_filesystem_roundtrip_test.c](fatfs_filesystem_roundtrip_test.c) | Teste automatizado de fatfs filesystem roundtrip. Ler os asserts e os stubs antes de interpretar cobertura; não equivale a ensaio físico. Entradas/símbolos para consulta: `io_sector`, `check_image`, `main`. |
| [input_temperature_test.c](input_temperature_test.c) | Teste automatizado de input temperature. Ler os asserts e os stubs antes de interpretar cobertura; não equivale a ensaio físico. Entradas/símbolos para consulta: `board_millis`, `board_encoder_start`, `board_encoder_counter`, `board_encoder_button_pressed`, `board_max31855_read32`, `step`, `main`. |
| [led_test.c](led_test.c) | Teste automatizado de led. Ler os asserts e os stubs antes de interpretar cobertura; não equivale a ensaio físico. Entradas/símbolos para consulta: `mock_clock`, `HAL_GPIO_WritePin`, `HAL_GPIO_Init`, `main`. |
| [manual_board_test.c](manual_board_test.c) | Teste automatizado de manual board. Ler os asserts e os stubs antes de interpretar cobertura; não equivale a ensaio físico. Entradas/símbolos para consulta: `record`, `mock_clock`, `HAL_Init`, `HAL_GPIO_WritePin`, `HAL_GPIO_Init`, `board_cube_clock_init`, `board_cube_gpio_init`, `board_cube_peripherals_init`, `HAL_GetTick`, `HAL_RCC_GetPCLK2Freq`, `DWT_US_Init`, `DWT_US_Delay`. |
| [oled_boot_test.c](oled_boot_test.c) | Teste automatizado de oled boot. Ler os asserts e os stubs antes de interpretar cobertura; não equivale a ensaio físico. Entradas/símbolos para consulta: `board_millis`, `board_oled_prepare`, `board_oled_controller`, `board_oled_column_offset`, `board_oled_write`, `main`. |
| [oled_profiles_test.c](oled_profiles_test.c) | Teste automatizado de oled profiles. Ler os asserts e os stubs antes de interpretar cobertura; não equivale a ensaio físico. Entradas/símbolos para consulta: `board_millis`, `board_oled_prepare`, `board_oled_controller`, `board_oled_column_offset`, `board_oled_write`, `main`. |
| [run_filesystems_test.py](run_filesystems_test.py) | Teste automatizado de run filesystems. Ler os asserts e os stubs antes de interpretar cobertura; não equivale a ensaio físico. |
| [run_tests.py](run_tests.py) | Executor de testes: configura ferramentas/compilações e reúne verificações. Conferir dependências e caminhos do ambiente. |
| [sd_sector_trace_test.c](sd_sector_trace_test.c) | Teste automatizado de sd sector trace. Ler os asserts e os stubs antes de interpretar cobertura; não equivale a ensaio físico. Entradas/símbolos para consulta: `main`. |
| [sdprobe_test.c](sdprobe_test.c) | Teste automatizado de sdprobe. Ler os asserts e os stubs antes de interpretar cobertura; não equivale a ensaio físico. Entradas/símbolos para consulta: `main`. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
