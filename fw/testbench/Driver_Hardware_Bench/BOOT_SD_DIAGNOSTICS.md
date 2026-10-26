# SD: validação manual

TESTAR SD RW no menu, ou comando 6 em g_bench_control, monta, cria um arquivo
novo, grava, sincroniza e relê. Não modifica os IOs de relé, SELECT ou LED.
Pressionamento longo é uma parada manual, não um teste de SD.
SDIO 1 bit/400 kHz, timeout de transferência 1 s e até três tentativas somente
para CRC de leitura de setor único. Estado em g_bench_status e g_sd_io_debug.
Falhas apenas são indicadas; não mudam o comando manual de potência.
Veja README.md. Hardware ainda precisa ser conferido na placa.

## Partida do OLED independente

O boot inicializa I2C/OLED e mostra o painel antes dos outros drivers da aplicação.
Não inicializa nem monta cartão automaticamente. SD começa como não testado;
selecione TESTAR SD RW (ou comando 6) para iniciar/montar/escrever/reler.
DWT indisponível é reportado, mas não impede a inicialização do OLED.
Refresh, Clean e Build no projeto Driver_Hardware_Bench e grave esse projeto.
As saídas continuam estritamente manuais, sem decisões baseadas em leituras.
Se a tela continuar apagada, pause e confira g_bench_status.loop_count,
g_bench_status.oled, g_hw_debug.oled_prepare_attempts e oled_last_hal_status.
