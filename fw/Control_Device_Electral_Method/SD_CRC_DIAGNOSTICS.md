# Falha de CRC ao ler o cartão

Valores reportados: read_attempts=1, read_hal_status=1 (HAL_ERROR),
read_hal_error=2 (HAL_SD_ERROR_DATA_CRC_FAIL), mount_result=1 (FR_DISK_ERR).
A assinatura do setor 0 só é atualizada após leitura bem-sucedida; seu valor
zero não comprova que o setor do cartão esteja zerado.

Atualização: até três tentativas de leitura de um único setor, somente se o
único erro for CRC de dados e o cartão responder em estado TRANSFER. Intervalo
entre tentativas: 2 ms. Demais falhas, transferências de múltiplos setores e
escritas continuam retornando falha. Nenhum CRC é ignorado.

Após encerrar Debug, Refresh, Clean, Build e gravar, deixe iniciar e pause.
Em Expressions, expanda g_sd_io_debug:

- read_hal_status / read_hal_error: resultado da última tentativa.
- read_retries: tentativas adicionais acumuladas.
- read_crc_failures: quantas leituras falharam por CRC.
- last_failed_read_error: preserva o último erro, mesmo se a repetição funcionar.
- first_read_signature: 21930 (0x55AA) após leitura válida do setor 0.

Confira g_hw_debug.sd_mount_result (0=montado). Se montar e os contadores de CRC
continuarem aumentando, a comunicação permanece instável; isso não comprova
correção do hardware. A causa ainda pode envolver contato no slot, alimentação
ou integridade do sinal. Em 1 bit, DAT0/PC8 é a linha de dados utilizada.
Com a placa desligada, confira encaixe/contatos do cartão e se PC8, PC12 ou PD2
não estão sendo usados por outro circuito. Não formate para tentar corrigir CRC.

Mantidos SD 1 bit/400 kHz, encoder -1 e a configuração atual de potência.
Testes do código real de leitura cobriram CRC transitório, três falhas,
cartão ocupado, timeout e múltiplos setores; regressões SD e build ARM passaram.
Nenhuma gravação/leitura física foi executada na placa pelo assistente.
