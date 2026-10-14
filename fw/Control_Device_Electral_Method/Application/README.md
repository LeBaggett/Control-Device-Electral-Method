# Thermal Board v6 — módulos de aplicação implementados

Continuação do v5, usando os dois documentos de planejamento fornecidos em
14/10/2026. A correção posterior para **MAX31855 / termopar** prevalece sobre
as referências antigas a MAX31865/PT100. A pinagem v4/v5 foi preservada.

## O que foi implementado

- OLED SH1106 128x64 I2C (perfil padrão) e perfil alternativo SSD1306.
  Framebuffer, fonte pequena/grande, ícones e envio incremental de páginas.
  O driver é direto em C, sem dependência de u8g2; a API mantém a biblioteca
  gráfica fora das outras camadas. É uma escolha de implementação diferente
  da biblioteca sugerida no planejamento, sem afirmar compatibilidade u8g2.
- Splash original `epd_bitmap_.bin`, 1024 bytes, XBM por linhas LSB primeiro.
  Incorporada sem modificar a arte, exibida durante a splash de 2 s.
- Home, menu Teste/Calibração/Voltar, edição, revisão, andamento, resultado,
  erro e aborto. Ajuste do passo em 0,5 °C, temperaturas 0–150 °C.
- Encoder TIM2 32 bits: quatro contagens por detente, debounce, clique
  ao soltar e longo de 1 s; proteção contra wrap e grandes saltos de contagem.
- ADC Vf em PC0 e VDDA por VREFINT. Média cooperativa, coleta solicitada
  para calibração e captura curta para o teste. Fallback nominal identificado.
- FatFs: CAL_001..999.CSV, TST_001..999.CSV sem sobrescrever ensaios; escrita
  exata e sync, detecção de erros e verificação periódica do estado do cartão.
  CALIB.TXT recarregado no boot, CRC e backup da calibração anterior.
- Matemática de alvos, regressão `Tj = m*Vf + T0`, R² e conversão Vf/VDDA.
- Calibração com soak contínuo, reiniciado se sair da tolerância; teste por
  alvo com SELECT → 30 us → ADC; timeout, sobretemperatura, falhas do sensor,
  erros SD/ADC e aborto. Power-safe em todas as saídas.
- Aplicação integrada com os drivers; headers sem HAL fora de board/.

As máquinas de calibração e teste são executadas ponta a ponta **com hardware
simulado** nos testes. No board real deste pacote, relé ON e SELECT heating
continuam retornando false. Uma sessão real é recusada com erro de potência:
não existe uma opção de configuração que remova esse bloqueio.

## O que ainda precisa acontecer

Este ZIP contém os módulos de fonte, não um projeto STM32CubeIDE gravável.
Ainda faltam `.ioc`, HAL/CMSIS/startup/linker e middleware gerados pelo CubeMX,
compilação ARM, conferência elétrica e testes na placa. A lista de fontes está
em `target_sources.list`. Integre conforme [o guia](board/INTEGRACAO_CUBEIDE.md).

`THERMAL_USE_FATFS` começa em **0** para compilar o código sem inventar uma
dependência FatFs. Quando o middleware real estiver integrado, configure 1.
Enquanto 0, SD aparece OFF e as sessões são recusadas. O backend FatFs de
valor 1 está implementado e foi compilado/testado com uma API simulada.

Confira controlador/resolução, offset do SH1106, reset do módulo, VDDA/VREF+,
impedância de PC0 e parâmetros do protocolo no hardware. Calibração salva só
é válida para o mesmo DUT, corrente de medição e cadeia ADC: esses dados não
podem ser deduzidos pelo firmware. Não usar CALIB.TXT de outro dispositivo.

O superloop é cooperativo, mas **não estritamente não bloqueante**: HAL de
SPI/I2C e FatFs/SD podem bloquear. Potência permanece bloqueada também por
esse motivo; liberar o aquecimento exige revisão de tempos de pior caso e
intertravamentos, além das verificações elétricas.

## Verificação

Execute `python tests/run_tests.py` ou `make test` com GCC no computador.
Veja [VALIDACAO.md](VALIDACAO.md) para evidências e limites.
As prévias em `preview/` foram renderizadas do framebuffer real dos testes C.
**Nenhuma validação física, gravação na placa ou compilação com a HAL real
foi realizada.**
