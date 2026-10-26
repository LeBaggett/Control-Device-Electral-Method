# Diagnóstico de leitura de setores

Projeto principal temporariamente em SDIO 1 bit, 400 kHz (divisor 118).
Encoder continua -1, potência permanece bloqueada. Nada foi formatado.

Encerre a sessão Debug, faça Refresh, Clean e Build; inicie nova sessão Debug,
grave e deixe executar até a tela inicial. Pause e use Expressions normal.
Adicione g_sd_io_debug e expanda, ou os campos completos:

- g_sd_io_debug.read_attempts
- g_sd_io_debug.read_hal_status
- g_sd_io_debug.read_hal_error
- g_sd_io_debug.read_sector
- g_sd_io_debug.last_card_state
- g_sd_io_debug.card_status_hal_error

read_attempts=0 indica que a falha antecedeu a leitura dos setores.
read_hal_status: 0=HAL_OK, 1=HAL_ERROR, 2=HAL_BUSY, 3=HAL_TIMEOUT.
read_hal_error é a máscara de erro HAL preservada no fim da leitura;
consulte Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_sdmmc.h.
Se a leitura retorna OK e montagem falha, confira last_card_state e
card_status_hal_error, pois o driver também espera estado de transferência.
first_read_signature=0x55AA indica assinatura do setor 0 lido com sucesso;
não prova que o volume todo esteja íntegro.

Confira também g_hw_debug.sd_phase e g_hw_debug.sd_mount_result.
FR_DISK_ERR (1) indica falha de acesso ao disco, não justifica formatar o cartão.
Se montar corretamente a 400 kHz, investigar margem de comunicação antes de
subir clock/voltar a 4 bits. Este modo é diagnóstico, não configuração final.
Testes host de SD e compilação/link ARM passaram. Sem validação física.
