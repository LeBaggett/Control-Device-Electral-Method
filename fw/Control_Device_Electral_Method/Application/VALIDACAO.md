# Verificações e limites — 14/10/2026

Doze executáveis de teste compilados e executados com sucesso no computador com GCC, C11,
`-Wall -Wextra -Werror`. `python tests/run_tests.py`.

| Teste | Cobertura |
|---|---|
| contract | Interfaces públicas consistentes |
| job | Calibração, soak, ajuste do exemplo, sequência do ensaio, abort, timeout, sensor, SD/ADC, potência recusada |
| power_max31855 | Regressão do intertravamento e decodificação de temperatura/fault |
| board | Ordem segura, latches antecipados, leitura SPI/timeout, erro DWT e bloqueio fixo |
| dwt | Guardas de clock/limite/contador parado com CMSIS simulado |
| math | Alvos, coeficientes m≈−357 e T0≈279, R², degeneração, Vf/VDDA, formatação e CRC |
| ui | SSD1306/SH1106 pages, snapshot de frame, cada pixel da logo, menu e encoder/debounce/wrap |
| adc | Média, captura, VREFINT, fallback, rail e falha de conversão |
| sdlog | FatFs simulado habilitado, nomes sem sobrescrever, sync, CRC/backup, escrita curta/remoção |
| sd_board | SD ausente não causa trap; seleção do bus e verificação periódica |
| app | Link da aplicação inteira com board/periféricos simulados, boot/menu, início sem SD e falha do sensor |
| require_cal | REQUIRE_CAL=1 recusa antes de tocar SD/power; UI avisa sem emitir início |

`board/peripheral_io.c` também é compilado contra o header HAL simulado para
verificar tipos/símbolos da interface. Esse header não é a HAL real.

As sete telas de prévia foram renderizadas do framebuffer produzido pelo
código C e inspecionadas. A splash foi verificada visualmente em quatro
interpretações; a legível é 128x64, linhas LSB-first. Seus 1024 bytes foram
incorporados sem alteração e os pixels são comparados no teste ui.

Pendentes: `.ioc` e middleware reais, compilação/link ARM, tempos no alvo,
consumo de stack/Flash/RAM, OLED/I2C, SD/disco, ADC e referência elétrica,
polos/pulls, reset, termopar e intertravamentos. Os testes de protocolo usam
fakes que permitem potência para verificar a lógica; o board real do pacote
permanece bloqueado e é verificado separadamente.

Não foi realizado nenhum teste de hardware nem prometida validação da
precisão de 30 us. FatFs síncrono e timeouts HAL exigem revisão antes de
habilitar potência. Esta versão contém a implementação de software, não
um firmware já homologado ou pronto para operar o DUT.
