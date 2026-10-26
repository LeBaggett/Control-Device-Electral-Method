# OLED_Only_F407 - guia de manutenção

Projeto isolado de diagnóstico do OLED no STM32F407. Não executa o ensaio térmico completo.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Subpastas

| Pasta | Responsabilidade |
|---|---|
| [Core](Core/README.md) | Projeto isolado de diagnóstico do OLED no STM32F407. Não executa o ensaio térmico completo. |
| [Drivers](Drivers/README.md) | Bibliotecas de periféricos do fabricante. Manter fontes e headers da mesma versão; registrar qualquer ajuste local antes de atualizar o pacote. |
| [firmware](firmware/README.md) | Estrutura auxiliar/legada de testes. O projeto ativo é Core + Application + FATFS + Drivers + Middlewares; conferir .cproject antes de adicionar fontes desta pasta ao build. |

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [.cproject](.cproject) | Metadados Eclipse/CubeIDE: identidade do projeto, toolchain, includes e exclusões de compilação. |
| [.project](.project) | Metadados Eclipse/CubeIDE: identidade do projeto, toolchain, includes e exclusões de compilação. |
| [OLED_Only_F407.launch](OLED_Only_F407.launch) | Configuração de execução/depuração do CubeIDE. Conferir ELF, ST-LINK e portas locais antes de reutilizar. |
| [STM32F407VETX_FLASH.ld](STM32F407VETX_FLASH.ld) | Script de ligação: regiões Flash/RAM, seções, heap e stack. Deve corresponder ao MCU e ao startup. |
| [STM32F407VETX_RAM.ld](STM32F407VETX_RAM.ld) | Script de ligação: regiões Flash/RAM, seções, heap e stack. Deve corresponder ao MCU e ao startup. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)


## Documentação anterior preservada

O conteúdo abaixo registra etapas anteriores. Em caso de divergência, conferir o guia atual acima e os fontes da versão correspondente.

<details>
<summary>Abrir documentação anterior</summary>

# Teste isolado OLED — STM32F407VET6

Projeto separado: `OLED_Only_F407`. Não substitui os projetos principal e Driver_Hardware_Bench. Não depende de SD, termopar, encoder, ADC, DWT ou cristal externo. Usa HSI 16 MHz e I2C1 a 100 kHz.

## Ligação

Com a alimentação desligada, conecte o OLED: GND ao GND da placa, VCC a 3,3 V, SCL a PB8 e SDA a PB9. Para este teste, deixe conectados somente ST-LINK, alimentação e OLED; desconecte a alimentação do estágio de potência. Confirme os nomes dos pinos no módulo, pois a ordem física varia.

PA1 é o LED da placa, ativo baixo. PD12 fica alto (medição) e PD14 baixo (relé desligado). Não existem comandos de aquecimento ou acionamento de relé neste projeto.

Os pinos, controlador e velocidade estão em `Core/Inc/pinout.h`. SSD1306 é o padrão; para SH1106 altere `OLED_CONTROLLER`. Endereços 0x3C e 0x3D são testados automaticamente. Os pull-ups internos são uma ajuda de diagnóstico; confirme pull-ups externos para **3,3 V** no módulo/circuito.

## Abrir e executar

1. CubeIDE: File → Import → General → Existing Projects into Workspace; selecione esta pasta.
2. Selecione **OLED_Only_F407**, faça Clean e Build.
3. Execute a configuração `OLED_Only_F407.launch` e confirme que ela grava `Debug/OLED_Only_F407.elf`.
4. Resume após parar em main. Não gere código pelo CubeMX: este projeto mínimo é mantido diretamente em C e não contém `.ioc`.

O display deve acender todos os pixels por dois segundos e depois apresentar um quadro com padrão quadriculado. PA1 pisca a cada 250 ms no laço principal. Esse teste inicial de todos os pixels independe do posicionamento do framebuffer.

## Expressions / Live Expressions

Adicione a estrutura inteira **`g_oled_test`**. Para registrar um diagnóstico consistente, pause o Debug depois de cinco segundos.

| Campo | Significado |
|---|---|
| `magic` | 0x4F4C4544 identifica este firmware |
| `heartbeat` | Contador do laço principal |
| `ready` | 1: todos os comandos e dados tiveram sucesso no I2C; não comprova emissão de luz |
| `failure` | 0: sem falha; 2: SCL baixo; 3: SDA baixo após até 9 pulsos; 4: HAL init; 5: nenhum ACK em 0x3C/0x3D; 6: comandos; 7: envio do padrão; 9: configuração de clock |
| `phase` | 1: liberação do barramento; 4: init; 5: endereço; 6: comandos; 7: padrão; 8: concluído |
| `scl_before`, `sda_before` | Níveis medidos com as saídas open-drain liberadas, antes da recuperação |
| `scl`, `sda` | Níveis no último diagnóstico |
| `hal_status`, `hal_error`, `hal_state` | Retorno, erro e estado HAL |
| `address_7bit` | Endereço encontrado, 60 (0x3C) ou 61 (0x3D) |
| `pulses` | Pulsos de recuperação, máximo 9 |

Para repetir sem reset: pause, escreva `g_oled_test.retry = 1` e continue. A recuperação só atua em PB8/PB9, em open-drain, respeitando SCL baixo e sem forçar nível alto. As falhas não disparam tentativas infinitas.

Se não aparecer imagem, envie **failure, scl_before, sda_before, hal_status, hal_error, address_7bit, ready**. O teste no F1 confirma o funcionamento do display naquela montagem; este teste verifica o caminho específico do F407.

Compilação ARM e testes simulados de endereços, linhas presas, recuperação limitada e falha de transmissão foram executados. Não houve gravação ou validação física da placa.

</details>
