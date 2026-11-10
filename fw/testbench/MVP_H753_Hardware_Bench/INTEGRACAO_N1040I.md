# N1040i — integração RS485 no testbench H753

## O que foi preparado

Firmware `0.2.2-n1040i`, na mesma bancada Ethernet. Primeiro conectar **um N1040i com RS485**, endereço 247. Leitura de PV, identificação, versão e estados; nenhuma escrita de configuração, acionamento de saída ou controle de aquecimento.

O servidor continua em `http://192.168.7.2/`. O painel N1040i consulta `GET /api/n1040`; a API anterior `/api/status` continua disponível. A página é incorporada na Flash com o firmware.

## Módulo com direção automática (padrão atual)

`BENCH_RS485_MANUAL_DIRECTION = 0`: usar somente TX, RX, VCC e GND no lado TTL. **PD4 não é configurado nem acionado**. O módulo deve comutar a direção por hardware; confirmar o modelo antes de presumir características elétricas. No lado TTL, saída TX do STM (PD5) vai à entrada RX do módulo; saída TX do módulo vai ao RX do STM (PD6), quando os rótulos são do ponto de vista do módulo. Alguns fabricantes usam rótulos de conexão ao host: consultar o esquema/foto do módulo.

No lado RS485, o módulo informado tem A+ e B−: usar a indicação de polaridade, A+ → D+ (borne 15) e B− → D− (borne 16), referência de comunicação → borne 17. Não usar apenas a letra A/B para inferir polaridade. VCC deve seguir a especificação do módulo; as etiquetas TX/RX não garantem lógica de 3,3 V.

Manter 115200 baud/8N1, endereço inicial 247. A mudança de direção automática não demonstra que o timeout está resolvido: verificar bytes/respostas na placa. Se o módulo devolver eco local, ele poderá exigir tratamento adicional; o firmware atual valida quadros recebidos e não os aceita como medição se tiverem formato incorreto.

Para módulo com DE e /RE expostos, selecionar `BENCH_RS485_MANUAL_DIRECTION = 1`; a ligação e o funcionamento descritos abaixo aplicam-se a esse modo.

## Ligação no modo manual

O STM32 precisa de um **transceptor RS485** entre a UART e os bornes do instrumento. Não conectar A/B diretamente à NUCLEO.

| NUCLEO / função | Transceptor |
|---|---|
| PD5 / USART2_TX | DI (entrada do transmissor) |
| PD6 / USART2_RX | RO (saída do receptor, com nível compatível com o STM32) |
| PD4 / direção | DE e /RE ligados juntos |
| GND | GND do módulo / referência da comunicação |

DE alto habilita o transmissor. /RE baixo habilita o receptor. PD4 baixo mantém recepção, PD4 alto habilita transmissão e desabilita recepção, evitando eco local. O firmware retorna PD4 a zero no evento **TC**, após sair o último stop bit. Recomenda-se pull-down externo de 10 kΩ na direção para manter o transmissor desligado durante reset.

No N1040i RA-485, conforme manual V2.0x fornecido:

| N1040i | Sinal |
|---|---|
| Borne 15 | D1 / D+ / B |
| Borne 16 | D0 / D− / A |
| Borne 17 | C / referência da comunicação |

**A/B têm convenções diferentes entre fabricantes.** Relacione o módulo por D+/D− e pela folha de dados; não presuma que o A de todo módulo corresponde ao borne A do N1040i. Use par trançado, barramento linear, terminação de 120 Ω apenas nas duas extremidades físicas e polarização em um ponto, considerando os resistores já existentes nos módulos. Não conectar 24 V ao GND de sinal por engano; alimentação do instrumento e comunicação são circuitos distintos.

**MAX485 clássico:** usa alimentação de 5 V. Não alimentá-lo com 3,3 V presumindo operação garantida. Antes de ligar RO ao STM32, verificar o circuito do módulo e compatibilidade de tensão; usar adaptação de nível se necessário. Uma alternativa de transceptor com lógica/alimentação de 3,3 V é MAX3485. O firmware é o mesmo para módulos half-duplex com DI, RO, DE e /RE; módulos com direção automática precisam de revisão do esquema de conexão. A alimentação de 24 V do modelo do N1040i não alimenta a NUCLEO nem o transceptor diretamente.

## Configurar no indicador

No teclado ou QuickTune, ajustar:

- `Addr = 247`.
- `bAud = 115200` (o valor 7 na tabela de registradores corresponde a 115200).
- `Prty = none`, sem paridade. Dados: 8 bits, 1 stop bit.
- `tYPE = Tc J` ou `Tc K`, conforme o termopar efetivamente conectado.
- Unidade em °C para os ensaios futuros. O painel também reconhece °F pelo Status Word 2 e não rotula o valor como °C incorretamente.

O firmware não escolhe J/K no instrumento e não aplica nova calibração. A conversão e a compensação de junta fria são do N1040i. Não usar outro mestre Modbus no mesmo barramento durante o teste.

## Atualizar e testar

1. Conectar a bancada sem DUT, aquecedor ou estágio de potência. Alimentar o N1040i conforme a etiqueta/manual do modelo recebido e verificar a leitura local.
2. No projeto `MVP_H753_Hardware_Bench`, atualizar arquivos (F5), Clean, Build e gravar novamente. Não gerar código CubeMX sobre esta pasta: esta etapa continua sem `.ioc`.
3. Executar/Resume (F8). PC Ethernet `192.168.7.1/24`, NUCLEO `192.168.7.2/24`; manter o Wi-Fi se desejado.
4. Abrir a página e atualizar sem cache (Ctrl+F5). Aguardar pelo menos um ciclo completo de estados + PV.
5. Comparar a temperatura web com o display, incluindo uma pequena variação e, se possível, valor negativo. PV é um inteiro assinado em décimos, independentemente da posição decimal do display.
6. Desconectar a comunicação: a leitura deve ficar indisponível, timeouts devem aumentar e a Ethernet deve continuar atendendo. Reconectar e verificar recuperação automática.
7. Testar termopar desconectado: verificar os estados do indicador e ausência de indicação de temperatura válida. Reconectar e comparar novamente com o display.
8. Testar duas abas e reconexão Ethernet enquanto a serial continua funcionando. Validar 30 minutos antes de expandir para três indicadores.

Sem módulo/instrumento conectado, timeouts são esperados e não devem acender o LED vermelho nem parar o servidor.

## Onde alterar e acompanhar

| Arquivo/variável | Conteúdo |
|---|---|
| `Application/board/pinout.h` | PD5 TX, PD6 RX, PD4 direção; mapeamento central |
| `Application/bench_config.h` | Enable, baud, paridade, quantidade, endereços e tempos |
| `Application/modbus_read.c/.h` | CRC16 e codificação/validação da função 03 |
| `Application/rs485_port.c/.h` | USART2, RX DMA circular, eventos IDLE/HT/TC, direção e recuperação |
| `Application/n1040.c/.h` | Sequenciamento das consultas, estados por instrumento e JSON |
| `Application/ui/index.html` | Visualização e orientações de diagnóstico |
| `tools/embed_web.py` | Atualizar `index_html.h` depois de editar o HTML; recompilar em seguida |
| `g_n1040_debug` | Diagnóstico completo do protocolo, `node[0]` é o primeiro indicador |
| `g_rs485_debug` | Inicialização UART/DMA, bytes, erros, reinícios e direção |

Padrão: uma requisição a cada 250 ms, timeout 400 ms, validade máxima 5 s. Dois pedidos alternados por instrumento: registrador 6, quantidade 5 (status/versão/ID), e registrador 1, quantidade 1 (PV). Um instrumento atualiza PV aproximadamente a cada 500 ms com respostas rápidas. Para três, configurar `BENCH_N1040_COUNT = 3` e endereços exclusivos 247, 246, 245, nesta ordem decrescente; ciclo nominal por instrumento aproximadamente 1,5 s. Testes de três nós foram feitos em simulação, ainda não fisicamente.

## Diagnóstico e causas possíveis

| Código `node[i].last_error` | Significado / investigação |
|---|---|
| 0 | Nenhuma falha de protocolo atual; ainda conferir `valid`, frescor e `status3` |
| 1 | Timeout: alimentação, fiação D+/D−, referência, endereço, baud, paridade e DE/RE |
| 2 | CRC: cabo, interferência, terminação/polarização, configurações seriais |
| 3 | Endereço inesperado: outro equipamento/mestre ou endereço incorreto |
| 4 | Função inesperada |
| 5 | Comprimento/byte count incompatível |
| 6 | Exceção Modbus; consultar `exception_code`: 1 função, 2 endereço, 3 quantidade |
| 7 | Quadro incompleto |
| 8 | Erro UART/transportador; conferir `g_rs485_debug.uart_error` |
| 9 | ID diferente de 68; conferir modelo e mapa da versão do instrumento |

`status3 & 0x7f` diferente de zero invalida a medição; o valor bruto fica disponível para diagnóstico. A interpretação exata dos bits deve seguir o manual da versão do instrumento. Os manuais NOVUS consultados têm descrições diferentes para o bit 6, portanto o firmware usa a máscara conservadora e mostra o estado bruto em vez de atribuir uma causa específica não confirmada.

Falha UART não é automaticamente defeito no transceptor; as mensagens indicam verificações possíveis. `uart_error` mantém o último código ocorrido e `uart_errors` é acumulativo, mesmo depois da recuperação. D-cache continua desligado e não há manutenção de cache no caminho RX Ethernet. O HardFault anterior foi corrigido e o usuário confirmou acesso HTTP na placa; isso não valida a nova serial.

## Estratégia e limites

- Um mestre e uma requisição pendente por vez: evita sobreposição entre escravos e permite ampliar para três sem RTOS.
- DMA1 Stream0/USART2_RX em buffer circular de 128 bytes na SRAM AXI, acessível ao DMA; nenhuma alocação dinâmica na serial.
- Interrupções apenas atualizam cursores/contadores ou devolvem a direção ao receptor. CRC, estados, JSON e recuperação ficam no superloop.
- IDLE notifica progresso; o frame só é entregue após silêncio adicional de pelo menos 3,5 caracteres, com tempo DWT. Não é finalizado apenas por IDLE de um caractere. Validação estrita de cada intervalo interno de 1,5 caracteres ainda não é implementada nesta bancada.
- TX de apenas 8 bytes por interrupção, sem espera por polling. Timeout de TX força recuperação se TC não chegar.
- Dados antigos não se tornam zero graus: precisam de PV e status recentes, ID 68 e ausência de flags de medição.
- O módulo usa D-cache OFF da etapa atual. Ativar D-cache exige tratar toda a coerência RX/TX; não basta inserir invalidação isolada.
- Este teste não implementa precisão temporal de aquisição TSP nem controle de temperatura. RTU não tem identificador de transação; o barramento deve ter apenas o mestre STM32 e responder dentro do timeout configurado.

## Verificação feita no computador

`python Application/tests/run_n1040_tests.py`: CRC com vetor conhecido, frames incorretos, complemento de dois, estados, ID, °F, timeout, exceção, erro UART, dados antigos, wrap de tempo, serialização limitada e round-robin com 1 e 3 nós simulados. Verifica também sintaxe JavaScript.

`python Application/tests/run_tests.py`: regressão HTTP/LwIP, buffers independentes, comprimento/JSON, falha de alocação, contadores e invariantes de potência/cache.

Compilação ARM do firmware foi executada. **Conexão elétrica, troca de direção, recepção DMA e leituras reais do N1040i ainda dependem dos testes acima na bancada.**

## Fontes

- Manual NOVUS N1040i V2.0x fornecido pelo usuário: `manual_n1040i_v20x_m_pt.pdf`, anexo 1, registradores e ligação.
- Protocolo oficial: https://cdn.novusautomation.com/downloads/n1040i_communication_protocol_v20x_en.pdf
- Manual oficial: https://cdn.novusautomation.com/downloads/manual_n1040i_v20x_m_en.pdf
- MAX3485: https://www.analog.com/en/products/max3485.html
