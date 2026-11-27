> Atualização 2026-11-27: ligação física corrigida pelo usuário para PD0/RX e PD1/TX. Firmware 0.2.7-hmi-pd01 usa GPIOD/AF8 e diagnóstico com os mesmos nomes do pinout.h. O mapeamento anterior PC10/PC11 também existe no MCU, mas não correspondia aos fios instalados. UART4/DMA1 Stream1, endereço 1 e 115200/8N1 preservados. Recompilar e gravar; conferir /api/hmi: tx_pin=PD1 e rx_pin=PD0. Não houve validação física por software nesta entrega.
>
> Referência: https://www.st.com/resource/en/datasheet/stm32h753zi.pdf (funções alternativas PD0/PD1).

# Integração UART4 — bancada 0.2.6-hmi-bench

PD1/TX e PD0/RX foram confirmados pelo usuário. O firmware em fw/testbench/MVP_H753_Hardware_Bench implementa UART4, DMA1 Stream1 circular e escravo Modbus RTU endereço 1, 115200 baud, 8N1. A IHM Delta é mestre e o conversor tem direção automática, sem GPIO DE/RE. Os NOVUS permanecem em USART2/DMA1 Stream0, endereços 247/246/245 e perfil rápido validado pelo usuário.

O mapa de 64 palavras e a especificação dos objetos estão em [screens/bench](screens/bench/README.md). FC03 e FC04 leem o mapa; escritas são rejeitadas. Offset 0 = 0xCA15; offset 3 = heartbeat em segundos; temperaturas assinadas em décimos nos offsets 11/21/31, validade em 12/22/32 e unidade em 19/29/39. Consultar o CSV para o mapa completo e conversão 40001/offset 0.

O serviço roda no superloop. IRQs apenas registram eventos e erros; recuperação HAL ocorre no laço principal. Buffers DMA em AXI SRAM, alinhados; D-cache continua desabilitado conforme a política da bancada. Não habilitar cache sem revisar coerência. Não há comandos de atuação ou inicialização de potência nesta integração.

Diagnóstico: /api/hmi, painel web IHM/UART4, g_hmi_debug e g_hmi_uart_debug. Zero requisições é esperado sem programa mestre na Delta. Bytes recebidos não comprovam frames válidos: comparar requests/responses/CRC e heartbeat na tela. Para desligar o serviço, BENCH_HMI_ENABLED=0 em Application/bench_config.h e recompilar.

DOPSoft 4.00.16 localizado e aberto. Falta confirmar a porta COM/DB9 cabeada, concluir o projeto nativo, compilar e transferir. Não houve teste físico da UART4, nem transferência para a Delta. Compilação ARM e testes de protocolo/mapa, NOVUS e HTTP passaram no computador. Há aviso preexistente de parâmetro não usado no driver LAN8742 do fornecedor.

O ponto anterior confirmado pelo usuário é 63759a5. Backup das fontes substituídas: MVP_before_UART4_20261123.zip no workspace de trabalho. POC preservada.

## Cabeamento informado e correção necessária

O usuário informou COM1 e bornes TXD/RXD. Estes correspondem ao modo RS232, incompatível com a entrada A/B do conversor RS485. Usar COM1 em modo RS485, DB9 pino 1 D+ para A+ e pino 6 D− para B−; conferir numeração pelo lado correto do conector e referência GND pino 5. Não conectar os níveis RS232 diretamente ao TTL da UART4. A correção física ainda não foi confirmada.

Fonte: https://filecenter.deltaww.com/Products/download/06/060302/Manual/DELTA_IA-HMI_DOP-107BV_TC-SC-EN-TUR_20191016.pdf

Build adicional no STM32CubeIDE 1.19.0 Debug: 0 erros, 0 avisos, incluindo os novos arquivos Application e interrupções UART4/DMA1 Stream1. O build CLI com flags adicionais aponta o aviso preexistente do LAN8742 descrito acima. Nenhum firmware foi gravado automaticamente.
