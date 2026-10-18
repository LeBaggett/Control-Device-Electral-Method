Menu CAL TERMOPAR: 3/4 pontos contra referência digital e ajuste de offset/filtro. Instruções em `../THERMOCOUPLE_MENU.md`.

Atualização do protocolo: CAL e HEAT capturam Vf ao detectar o alvo, com espera de 30 us após SELECT, sem soak. Veja `../IMMEDIATE_VF_CAPTURE.md`.

# Thermal Board v6 â€” mÃ³dulos de aplicaÃ§Ã£o implementados

ContinuaÃ§Ã£o do v5, usando os dois documentos de planejamento fornecidos em
14/10/2026. A correÃ§Ã£o posterior para **MAX31855 / termopar** prevalece sobre
as referÃªncias antigas a MAX31865/PT100. A pinagem v4/v5 foi preservada.

## O que foi implementado

- OLED SSD1306 128x64 I2C (perfil padrÃ£o) e perfil alternativo SH1106; seleÃ§Ã£o em `board/pinout.h`.
  Framebuffer, fonte pequena/grande, Ã­cones e envio incremental de pÃ¡ginas.
  O driver Ã© direto em C, sem dependÃªncia de u8g2; a API mantÃ©m a biblioteca
  grÃ¡fica fora das outras camadas. Ã‰ uma escolha de implementaÃ§Ã£o diferente
  da biblioteca sugerida no planejamento, sem afirmar compatibilidade u8g2.
- Splash original `epd_bitmap_.bin`, 1024 bytes, XBM por linhas LSB primeiro.
  Incorporada sem modificar a arte, exibida durante a splash de 2 s.
- Home, menu Teste/CalibraÃ§Ã£o/Voltar, ediÃ§Ã£o, revisÃ£o, andamento, resultado,
  erro e aborto. Ajuste do passo em 0,5 Â°C, temperaturas 0â€“150 Â°C.
- Encoder TIM2 32 bits: quatro contagens por detente, debounce, clique
  ao soltar e longo de 1 s; proteÃ§Ã£o contra wrap e grandes saltos de contagem.
- ADC Vf em PC0 e VDDA por VREFINT. MÃ©dia cooperativa, coleta solicitada
  para calibraÃ§Ã£o e captura curta para o teste. Fallback nominal identificado.
- FatFs: CAL_001..999.CSV, TST_001..999.CSV sem sobrescrever ensaios; escrita
  exata e sync, detecÃ§Ã£o de erros e verificaÃ§Ã£o periÃ³dica do estado do cartÃ£o.
  CALIB.TXT recarregado no boot, CRC e backup da calibraÃ§Ã£o anterior.
- MatemÃ¡tica de alvos, regressÃ£o `Tj = m*Vf + T0`, RÂ² e conversÃ£o Vf/VDDA.
- CalibraÃ§Ã£o com soak contÃ­nuo, reiniciado se sair da tolerÃ¢ncia; teste por
  alvo com SELECT â†’ 30 us â†’ ADC; timeout, sobretemperatura, falhas do sensor,
  erros SD/ADC e aborto. Power-safe em todas as saÃ­das.
- AplicaÃ§Ã£o integrada com os drivers; headers sem HAL fora de board/.

As mÃ¡quinas de calibraÃ§Ã£o e teste sÃ£o executadas ponta a ponta **com hardware
simulado** nos testes. No board real deste pacote, relÃ© ON e SELECT heating
retornam false por padrÃ£o. A atualizaÃ§Ã£o de 15/10/2026 adiciona
BOARD_POWER_CONTROL_ENABLED em board/pinout.h, comeÃ§ando em 0. O valor 1
habilita os comandos com intertravamento apÃ³s recompilar; nÃ£o inicia
automaticamente uma sessÃ£o. Veja docs/CONTROLE_POTENCIA.md.

## O que ainda precisa acontecer

Este ZIP contÃ©m os mÃ³dulos de fonte, nÃ£o um projeto STM32CubeIDE gravÃ¡vel.
Ainda faltam `.ioc`, HAL/CMSIS/startup/linker e middleware gerados pelo CubeMX,
compilaÃ§Ã£o ARM, conferÃªncia elÃ©trica e testes na placa. A lista de fontes estÃ¡
em `target_sources.list`. Integre conforme [o guia](board/INTEGRACAO_CUBEIDE.md).

`THERMAL_USE_FATFS` comeÃ§a em **0** para compilar o cÃ³digo sem inventar uma
dependÃªncia FatFs. Quando o middleware real estiver integrado, configure 1.
Enquanto 0, SD aparece OFF e as sessÃµes sÃ£o recusadas. O backend FatFs de
valor 1 estÃ¡ implementado e foi compilado/testado com uma API simulada.

Confira controlador/resoluÃ§Ã£o, offset do SH1106, reset do mÃ³dulo, VDDA/VREF+,
impedÃ¢ncia de PC0 e parÃ¢metros do protocolo no hardware. CalibraÃ§Ã£o salva sÃ³
Ã© vÃ¡lida para o mesmo DUT, corrente de mediÃ§Ã£o e cadeia ADC: esses dados nÃ£o
podem ser deduzidos pelo firmware. NÃ£o usar CALIB.TXT de outro dispositivo.

O superloop Ã© cooperativo, mas **nÃ£o estritamente nÃ£o bloqueante**: HAL de
SPI/I2C e FatFs/SD podem bloquear. PotÃªncia permanece bloqueada tambÃ©m por
esse motivo; liberar o aquecimento exige revisÃ£o de tempos de pior caso e
intertravamentos, alÃ©m das verificaÃ§Ãµes elÃ©tricas.

## VerificaÃ§Ã£o

Execute `python tests/run_tests.py` ou `make test` com GCC no computador.
Veja [VALIDACAO.md](VALIDACAO.md) para evidÃªncias e limites.
As prÃ©vias em `preview/` foram renderizadas do framebuffer real dos testes C.
**Nenhuma validaÃ§Ã£o fÃ­sica, gravaÃ§Ã£o na placa ou compilaÃ§Ã£o com a HAL real
foi realizada.**
