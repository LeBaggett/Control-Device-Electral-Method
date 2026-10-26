Esta bancada agora permite controle manual: siga README.md para g_bench_control e proteções.

# Encoder, temperatura e Live Expressions

Atualização aplicada aos projetos principal e de bancada, com acionamento manual autorizado e partida segura.
Faça Refresh (F5), Project > Clean e Build Project; grave pelo seu fluxo de debug.
Não precisa regenerar o CubeMX. O ajuste dos pull-ups está em região USER CODE.

## Acompanhar os dados

Com firmware Debug rodando, abra Live Expressions e adicione:

- `g_hw_debug` (expanda a estrutura)
- `g_hw_tuning` (ajustes de software)
- `g_bench_status` (somente no projeto de bancada)

Campos principais de `g_hw_debug`:

| Campo | Significado |
|---|---|
| tc_raw_c | Última temperatura bruta válida, °C; NAN em falha |
| tc_display_c | Temperatura suavizada usada no OLED |
| tc_internal_c | Temperatura interna/junta fria do MAX31855 |
| tc_min_c / tc_max_c | Extremos das leituras brutas desde inicialização do sensor |
| tc_frame | Quadro SPI de 32 bits; veja em hexadecimal |
| tc_valid / tc_status | Validade e resultado da última leitura |
| tc_samples / tc_errors / tc_last_ok_ms | Contagem de tentativas, erros e tempo do último sucesso |
| encoder_counter / encoder_last_counter | Contador TIM2 no snapshot / última consulta pelo driver |
| encoder_delta / encoder_remainder | Variação mais recente e contagens ainda não convertidas em evento |
| encoder_cw / encoder_ccw / encoder_clicks / encoder_long | Eventos acumulados |
| encoder_a / encoder_b / encoder_button_pressed | Níveis físicos A/B e botão pressionado |
| relay_level / select_level / tc_cs_level | Níveis lógicos dos pinos, não confirmação elétrica da carga |
| gpio_a_idr ... gpio_e_idr | Entradas digitais de cada porta, um bit por pino |
| gpio_a_odr ... gpio_e_odr | Registradores de saída; não substituem medição elétrica |
| vf_v / vdda_v | Tensão Vf convertida e alimentação ADC estimada |
| oled_ready / storage_ready / board_ready / power_enabled | Estados de software |
| updated_ms | Horário do último snapshot de GPIO (intervalo 50 ms) |

SPI/I2C/SDIO são rápidos: snapshots de GPIO não capturam seus pulsos. Para isso
use osciloscópio/analisador lógico. GPIOC bit 0 é um pino analógico: seu IDR não
representa a tensão Vf; acompanhe vf_v. Campos são atualizados por tarefas diferentes,
portanto a estrutura não é um snapshot simultâneo de todo o sistema.

`tc_status`: 0=OK, 1=termopar aberto, 2=curto GND, 3=curto VCC,
4=quadro inválido, 5=falha SPI. A temperatura bruta continua alimentando o controle,
as verificações de segurança e os registros; só a apresentação é filtrada.
Falhas deixam a temperatura exibida inválida imediatamente, sem manter valor antigo.

## Ajustar o encoder

Padrão alterado de 4 para **2 contagens por evento**, sem limite de tempo entre
contagens parciais. Giro lento não expira. Debounce do botão: **15 ms**.
Pull-ups internos A/B ativados para contatos que fecham a GND; configuração
`BOARD_ENCODER_PULLUPS_ENABLED` em `Application/board/pinout.h`.

Valores persistentes ficam em `Application/board/runtime_config.h`:

- THERMAL_ENCODER_COUNTS_PER_DETENT: 2 inicialmente; use 4 se um clique mecânico
  avançar duas opções, ou 1 se dois cliques mecânicos ainda gerarem só um evento.
- THERMAL_ENCODER_DIRECTION: 1 ou -1 para inverter a direção.
- THERMAL_BUTTON_DEBOUNCE_MS: 15; afeta somente o botão.
- THERMAL_DISPLAY_FILTER_ALPHA: 0.25 inicialmente.

Para experimentar sem recompilar, pause o debug e altere os campos de
`g_hw_tuning` em Expressions/Memory; depois retome. As alterações em RAM se perdem
no reset. Confirme o resultado no header e recompile para persistir.
Counts aceitos: 1 a 8; direção: +/-1; debounce: 0 a 100 ms.
Observe encoder_counter antes e após um clique mecânico completo, sem inverter
a direção. A diferença absoluta ajuda a escolher counts_per_detent. Não existe
um número universal para todos os encoders. Delta instantâneo geralmente fica em
zero durante o repouso; use os contadores acumulados para observar eventos.

## Temperatura oscilando

Filtro de tela: y = y + alpha*(leitura-y), a cada amostra (~250 ms).
Alpha=0.25 suaviza com atraso de resposta; 0.1 suaviza mais; 1 desativa a suavização.
Valores inválidos também desativam a suavização. Isso não calibra o sensor.
Para corrigir erro absoluto é necessária comparação com referência conhecida,
em equilíbrio térmico; não foi adicionado offset arbitrário.

Compare tc_raw_c e tc_display_c com o termopar imóvel. Se a bruta continuar
oscilando alguns graus, confira conexões e ruído no módulo. O fabricante recomenda
100 nF próximos de VCC/GND e capacitor diferencial de 10 nF entre T+ e T-, próximo
ao CI; confira o circuito do seu módulo antes de acrescentar componentes.
Fonte: https://www.analog.com/media/en/technical-documentation/data-sheets/MAX31855.pdf

Validação desta atualização: testes host de giro lento, contagens 2/4, wrap do
TIM2, inversão, bounce de botão, filtro, leitura bruta preservada e falhas;
regressões dos dois projetos e compilação/link ARM. Não houve validação física.
