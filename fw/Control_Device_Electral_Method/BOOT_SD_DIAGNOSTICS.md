# Diagnóstico de partida: OLED, SD e termopar

Placa informada: DevEBox STM32F4XX M / STM32F407VET6.
https://stm32-base.org/boards/STM32F407VET6-STM32F4XX-M
O slot descrito usa PC8/9/10/11=DAT0/1/2/3, PC12=CLK, PD2=CMD,
com pull-ups externos; os pinos do projeto coincidem. Nenhum pino foi trocado.

## Aplicar e testar

Refresh, Clean, Build e grave o firmware. Teste também desligando totalmente a
alimentação e ligando novamente, com o cartão já inserido, sem apertar Reset.
Aguardar alguns segundos: SD e OLED têm tempos de estabilização e timeout.
Relé e SELECT continuam bloqueados. Nenhuma formatação ou escrita de teste foi
adicionada ao projeto principal.

FatFs: timeout de cada transfer�ncia em polling reduzido de 30 s para 1 s.
Um erro ainda pode bloquear por esse tempo; n�o � opera��o ass�ncrona.

OLED: espera inicial de 350 ms; consulta de presença I2C com até 3 trials;
até 3 reinicializações adicionais, espaçadas por pelo menos 1 segundo.
Recuperação somente fora de um ensaio, pois a espera de estabilização bloqueia.
No benchmark também há recuperação automática. Isso é uma tentativa de corrigir
partida a frio; não comprova que a alimentação e o módulo estão corretos.

SD: espera 300 ms antes de inicializar; identificação em 1 bit, depois 4 bits;
clock de transferência reduzido de 12 MHz para 4 MHz. Inicialização não trava em
Error_Handler ao faltar cartão. Fora de um ensaio, um pressionamento LONGO do
encoder refaz inicialização, montagem e carregamento da calibração do SD.
Não retire o cartão durante gravação/ensaio. O teste do benchmark por long press
continua criando um arquivo novo e verificando a leitura; no principal só remonta.

## Live Expressions

Expanda g_hw_debug:

| Campo | Significado |
|---|---|
| sd_attempts | Tentativas de inicialização |
| sd_phase | Etapa descrita abaixo |
| sd_init_status | Resultado HAL_SD_Init: 0=OK, 1=ERROR, 2=BUSY, 3=TIMEOUT |
| sd_wide_status | Resultado de configurar 4 bits; 0xFFFFFFFF=não executado |
| sd_hal_error | Máscara HAL de erro; use hexadecimal e stm32f4xx_hal_sd.h |
| sd_bus_bits | Largura negociada: 1 ou 4 |
| sd_card_state | Última consulta de estado do cartão |
| sd_mount_result | Resultado FatFs; -1=não tentou, 0=OK, 1=erro de disco, 3=não pronto, 13=sem volume FAT reconhecido |
| oled_prepare_attempts | Quantas inicializações I2C foram tentadas |
| oled_last_hal_status | Último resultado HAL I2C (mesmos códigos HAL acima) |
| oled_write_errors | Falhas ao enviar comandos/dados |
| oled_ready / storage_ready | Estados da aplicação |

sd_phase: 0=nunca tentou; 1=inicializando; 2=falhou HAL_SD_Init;
3=configurando 4 bits; 4=falhou configuração 4 bits; 5=inicialização concluída;
6=perdeu estado de transferência posteriormente.

sd_phase=5 e sd_mount_result diferente de 0 separa inicialização física e montagem.
FAT32 é suportado nesta configuração; exFAT está desativado. Código 13 também pode
indicar volume/partição inválidos ou leituras incorretas, não prova exFAT.
Não reformate antes de preservar dados e confirmar o diagnóstico.

Para um teste de isolamento, altere em Application/board/runtime_config.h:
`THERMAL_SD_BUS_BITS` de 4U para 1U e recompile. O padrão continua 4 bits.
Se 1 bit funciona e 4 bits não, investigar DAT1/2/3 e integridade de sinais.
`THERMAL_SD_CLOCK_DIV` controla clock de transferência: 48 MHz/(divisor+2).
Não foram adicionadas tentativas automáticas contínuas de SD, para não congelar
periodicamente a interface quando não há cartão.

## Termopar indicando queda ao aquecer a ponta

Confirme que aqueceu a ponta, não o módulo MAX31855. O fabricante descreve esse
comportamento para polaridade T+/T- invertida. Com tudo desligado, confira os
terminais e a polaridade da sonda; teste novamente com o módulo longe da mão.
Não foi invertido sinal nem aplicado offset por software. Compare tc_raw_c com
tc_internal_c no Live Expressions. Se continuar invertido, conferir tipo da
sonda e versão do MAX31855 antes de usar para controle térmico.
Fonte: https://ez.analog.com/data_converters/a/documents/c/max31855kasa-t-faq/DO12711/what-is-the-temperature-conversion-behavior-of-the-max31855-if-the-thermocouple-leads-are-connected-in-reverse

Verificação de software: regressões, inicialização SD sem cartão e falha de 4 bits,
modo 1 bit, OLED que responde tarde, recuperação e limite de tentativas, compilação
ARM dos dois projetos. Não houve validação física do OLED/SD/termopar.
