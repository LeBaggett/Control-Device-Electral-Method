# Guia para desenvolver telas — Delta DOP-107BV

Contrato de comunicação da bancada CAIS / NUCLEO-H753ZI. Revisão: 2026-11-27.
Firmware de referência: **0.3.0-adc-dma**, mapa versão **2**.

## Objetivo e autonomia do responsável pelas telas

Este documento contém as informações necessárias para criar páginas, reorganizar o visual e adicionar objetos no DOPSoft **sem acessar, recompilar ou gravar o firmware da NUCLEO**. A interface é um mapa fixo de registradores: qualquer tela pode consultar os mesmos dados.

Para adicionar uma página, o trabalho é feito somente no projeto da Delta: criar a tela, inserir os objetos, configurar os endereços publicados aqui, criar a navegação, compilar e transferir para a IHM. Não se cadastram números de tela, imagens, botões de navegação ou layouts no STM32. As artes podem ser fundos; as leituras e os botões precisam de objetos nativos do DOPSoft sobrepostos.

A autonomia cobre os dados publicados no mapa versão 2. Não existem comandos de iniciar/parar calibração ou ensaio, ajustes de corrente, atuação de relé/aquecedor ou saídas DAC nesta interface de bancada. Criar um desenho de botão não implementa essas funções. Um dado ou comando novo, fora deste contrato, deve ser combinado com o responsável pelo firmware antes de ser usado.

## Estado verificado e escopo

O usuário confirmou comunicação entre Delta e STM32: 1631 consultas e 1631 respostas, sem erros de UART, CRC, exceções ou transmissão no diagnóstico informado. Também confirmou que W40004 aumenta na tela. A causa da falha anterior foi o mapeamento físico do adaptador DB9 para bornes. Isso registra a validação relatada pelo usuário; não equivale a teste prolongado, de todos os registradores ou de todas as futuras telas.

O programa nativo existente deve ser tratado como trabalho em andamento, não como template integralmente homologado. Cada alteração de tela deve ser compilada e conferida na IHM.

## Comunicação que deve permanecer configurada

A Delta é **mestre Modbus RTU** e inicia as consultas. O STM32 é **escravo 1** e só responde. Os endereços 247, 246 e 245 identificam os NOVUS em outro barramento; **não são estações a consultar pela Delta**. Para os três canais, a estação da Delta continua sendo 1.

Em **General → Communication Settings**, configure:

| Campo | Valor |
|---|---|
| Modelo do projeto | DOP-107BV, 800 × 480 |
| Canal | COM1, Connection habilitado |
| Link Name | Link1 (nome adotado neste guia) |
| Manufacturers | Modbus |
| Series | 984 RTU (Master) |
| Interface | RS485 |
| PLC Station | 1 |
| Baud Rate | 9600 |
| Data Bits / Parity / Stop Bits | 8 Bits / None / 1 Bit |
| Connection Timeout | 1000 ms, valor inicial |
| Connect Retries | 3, valor inicial |
| Comm. Delay Time | 5 ms, valor inicial |
| Optimize | Desmarcado inicialmente |

O campo HMI Station pode ficar no padrão do driver; quem identifica o STM32 escravo é **PLC Station = 1**. Não configurar áreas externas automáticas de controle/status, macros ou outros objetos em W40100/W40200: esses endereços estão fora do mapa implementado. Não habilitar escritas de controle do driver.

Começar com atualização visual de aproximadamente 500 ms, onde o objeto/driver oferecer esse ajuste. A 9600 baud, o tempo cresce com a quantidade de objetos e consultas; 5 ms de Comm. Delay não significa que todos os dados são atualizados a cada 5 ms. Evitar consultas redundantes, históricos muito rápidos e macros de polling. A cadência real deve ser verificada na tela e nos diagnósticos. Manter apenas um mestre no barramento; retirar o mestre de teste do PC antes de ligar a Delta ao STM32.

### Referência de cabeamento (não precisa mudar ao criar telas)

| Delta COM1 / DB9 | Conversor RS485/TTL |
|---|---|
| Pino 1, D+ | A+ |
| Pino 6, D− | B− |
| Pino 5, GND | GND do lado RS485 |

No lado TTL da montagem validada: **TXD do módulo → PD1/TX** e **RXD do módulo → PD0/RX**, com GND comum. Essa nomenclatura é a do módulo automático usado no projeto; não generalizar para outro transceptor sem conferir suas direções elétricas. O módulo usa alimentação de 5 V na bancada. Nunca aplicar 24 V nos sinais do STM32 ou do conversor.

Conferir a numeração física do DB9, inclusive o lado de observação: a face frontal e o lado de solda/borne ficam espelhados. Não usar uma tabela genérica RS232/RS485 de outro equipamento. A USB de programação da Delta é uma conexão separada do canal de dados RS485.

## Como solicitar um dado no DOPSoft

Não é necessário escrever frames, CRC ou macros: o driver Modbus faz isso para cada objeto que tem endereço de leitura.

1. Inserir **Elements → Data Display → Numeric Display**.
2. Dar dois cliques no objeto e abrir **Read Address → …**.
3. Selecionar **Link1**, **Device (Word)** e **Device Type = W**.
4. Em **Address/Value**, digitar apenas o número, por exemplo **40004**. Não digitar `W`, `{Link1}` ou o offset de protocolo nesse campo.
5. Usar a estação padrão do link (**1**), ou selecionar explicitamente a estação 1.
6. Confirmar pelo **Enter** da janela de endereço e depois pelo OK do objeto.
7. Configurar tipo, formato e casas decimais conforme a tabela do dado.

O editor pode exibir o endereço resultante como `{Link1}1@W40004`: isso é normal. Essa expressão identifica link, estação e registrador; não deve ser colada inteira no campo numérico Address/Value.

### Regra de endereçamento

Para o driver **984 RTU (Master)** usado aqui:

**Endereço DOPSoft = 40001 + offset do protocolo**.

Exemplo: W40004 corresponde ao offset 3 no frame Modbus, não ao endereço binário 40004. Todos os dados são palavras de 16 bits. Não selecionar Double Word, Float ou troca de palavras. Os bytes de cada palavra são enviados com o mais significativo primeiro; o CRC é calculado pelo driver.

## Objetos prontos para as três temperaturas

| Canal visual | Temperatura | Validade | Online | Unidade | Idade da leitura (ms) |
|---|---|---|---|---|---|
| NOVUS 247 / canal 1 | W40012 | W40013 | W40014 | W40020 | W40017 |
| NOVUS 246 / canal 2 | W40022 | W40023 | W40024 | W40030 | W40027 |
| NOVUS 245 / canal 3 | W40032 | W40033 | W40034 | W40040 | W40037 |

Os nomes canal 1/2/3 seguem a ordem dos endereços. Não atribuir caixa, ambiente ou bloco a um canal sem a equipe confirmar a posição física do sensor.

### Temperatura: Numeric Display

| Propriedade | Configuração |
|---|---|
| Data Type | Word |
| Data Format | Signed Decimal |
| Integer Digits | 4 |
| Fractional | 1 |
| Gain / Offset | 1.0 / 0.0 |

Selecionar primeiro 4 dígitos inteiros e depois 1 fracionário. O valor recebido está em décimos: **297 → 29,7** e **−55 → −5,5**. Não aplicar ganho adicional de 0,1, pois a posição decimal já faz a apresentação esperada. Não exibir como unsigned: temperaturas negativas seriam interpretadas incorretamente.

### Validade e online: indicadores de palavra

Inserir um **Multistate Indicator** configurado para ler **uma palavra**, dois estados, no endereço de validade do canal:

- Estado 0: **SEM LEITURA VÁLIDA**, cinza/vermelho.
- Estado 1: **LEITURA VÁLIDA**, verde.

São registradores de palavra com valor 0 ou 1, não bobinas Modbus. Não configurar um endereço de bit B como substituto.

`online = 1` indica comunicação com o NOVUS; não garante que o sensor esteja válido. `valid = 1` também depende de PV/status recentes (limite atual 5 s), ausência de erro de comunicação, identificação esperada do instrumento e status do sensor. Usar **valid** para decidir a apresentação da temperatura. Se zero, ocultar o valor ou mostrar “—”, junto do indicador de falha. A idade 65535 significa leitura ausente ou idade saturada; não é temperatura.

### Unidade: indicador de palavra

No registrador de unidade de cada canal, usar dois estados: **0 = °C** e **1 = °F**. Mostrar a unidade recebida, sem conversão automática adicional. Evitar gravar “°C” permanentemente na imagem de fundo se o indicador puder ser configurado em Fahrenheit.

### Falha do próprio link IHM–STM32

Se a IHM perder comunicação, inclusive o registrador `valid` pode ficar com o último valor recebido. Portanto, habilitar a indicação de erro de comunicação do driver e sinalizar/ocultar valores retidos quando houver perda do link. Apenas mostrar um indicador verde vindo de um registrador não é suficiente.

O heartbeat W40004 deve avançar enquanto a comunicação estiver funcionando. Não usar apenas uma mudança no valor da temperatura como sinal de conexão: a temperatura pode permanecer constante.

## Diagnósticos úteis para outras telas

| Dado | Endereço | Apresentação |
|---|---|---|
| Assinatura do dispositivo | W40001 | Hexadecimal, esperado CA15 (51733 decimal) |
| Versão do mapa | W40002 | Inteiro, esperado 2 |
| Versão minor/patch do firmware | W40003 | Hexadecimal, 0300 nesta versão |
| Tempo ligado / heartbeat | W40004 | Unsigned Decimal, 5 inteiros, 0 decimais |
| Quantidade de NOVUS configurados | W40006 | Inteiro, atualmente 3 |
| Quantidade de canais válidos | W40007 | Inteiro, 0…3 |
| Consultas da IHM recebidas pelo STM32 | W40009 | Inteiro, parte baixa do contador |
| Último erro UART da IHM | W40043 | Máscara de bits HAL |
| Contagem de erros UART da IHM | W40044 | Inteiro, parte baixa do contador |
| Link físico Ethernet | W40047 | Estado 0 = desconectado; 1 = conectado |

O link Ethernet não informa o estado da comunicação serial da Delta nem garante que uma sessão HTTP esteja aberta. W40009 pode refletir a contagem anterior à consulta atual, porque o snapshot é montado antes de contar esse pedido.

Os contadores de 16 bits voltam a zero após 65535. W40004 também não é um relógio permanente: reinicia com a NUCLEO e repete após 65536 segundos (18 h 12 min 16 s). Não interpretar esse retorno como falha de comunicação. O tempo-base de 32 bits do firmware também reinicia seu ciclo aproximadamente a cada 49,7 dias; esta interface não é um registro histórico de uptime.

## Limites do contrato de leitura

- Faixa disponível: **W40001 a W40064**, offsets 0…63.
- Usar **FC03 / Read Holding Registers**. FC04 também lê o mesmo mapa nesta bancada, mas não é necessário mudar o driver.
- Cada consulta pode ler **1 a 64 palavras**, sem ultrapassar o fim do mapa. Por exemplo, offset 10 com 30 palavras é válido; offset 60 com 8 não é.
- Todos os campos são somente leitura. Não usar Set Value, Numeric Input, bobinas ou escritas Modbus para controlar o dispositivo.
- FC não implementada recebe exceção 01; faixa inválida recebe 02; quantidade zero ou maior que 64 recebe 03. Broadcast/endereço de outra estação e CRC incorreto não recebem resposta. O número de erro exibido pela Delta não deve ser automaticamente interpretado como código de exceção Modbus.
- W40048…W40064 são reservados e retornam zero; não significam canais adicionais implementados.
- Não habilitar agrupamento automático que ultrapasse o mapa. Se ativar Optimize futuramente, verificar as consultas geradas e os limites antes da entrega.

### Exemplo para quem usar outra ferramenta Modbus

Leitura do heartbeat: estação 1, FC03, offset 3, quantidade 1.

```text
Pedido RTU:  01 03 00 03 00 01 74 0A
Resposta de exemplo para 342 segundos:
             01 03 02 01 56 39 EA
```

O valor de resposta muda com o tempo. Em DOPSoft, basta configurar W40004; não implementar esse frame manualmente.

## Fluxo para acrescentar uma tela sem mexer na NUCLEO

1. Preservar uma cópia do projeto DOPSoft que já comunica.
2. Criar uma nova tela para DOP-107BV, 800 × 480, com nome e número próprios.
3. Inserir fundo e objetos nativos. Reutilizar Link1/estação 1 e os endereços deste guia; não criar outro mestre ou trocar a COM a cada página.
4. Criar botões **Goto Screen** para navegar entre páginas. Esses botões são locais da Delta e não precisam de endereço Modbus.
5. Para configurações da própria IHM, usar **System Menu**; para brilho, **Contrast Brightness**. Não mapear esses ajustes para registradores do STM32.
6. Conferir propriedades dos objetos copiados: endereço, canal, signed/unsigned, decimal, unidade e indicação de validade. Duplicar um objeto também duplica seu endereço.
7. Salvar, executar **Project → Compile All** e corrigir todos os erros de endereço.
8. Transferir **PC → IHM por Download All Data**, com IHM alimentada em 24 V e conexão de programação USB reconhecida. Upload All Data é o sentido IHM → PC. Não confundir com Download Boot Screen, que altera a inicialização.
9. Verificar W40004 avançando, os três valores nos instrumentos físicos, unidade e resposta a falhas de sensor/link. Nenhuma gravação do STM32 é necessária para uma mudança apenas de tela.
10. Entregar o projeto nativo editável, os recursos usados e uma nota da revisão do mapa; manter os arquivos na pasta hmi/mvp do repositório.

## Diagnóstico no PC e aceitação

Na rede da bancada, abrir **http://192.168.7.2/api/hmi**. `baud=9600`, `address=1`, `tx_pin=PD1`, `rx_pin=PD0` devem corresponder à montagem. `requests` e `responses` devem crescer, sem aumento de `crc_errors`, `exceptions`, `tx_failures` ou `uart_errors`. `responses` contabiliza envios aceitos pelo transporte; confirmar também o heartbeat na Delta, pois o contador isolado não comprova que a IHM recebeu e apresentou o dado.

| Sintoma | Verificação inicial |
|---|---|
| Element Address Input Error ao compilar/editar | Campo vazio ou sintaxe; usar Device Word/W e só o número do endereço |
| rx_bytes fica zero | Tela deve consultar; confirmar configuração transferida, COM, pinagem DB9 e jumpers |
| Erros UART 2 / 4 / 6 | Indicadores HAL de ruído / enquadramento / ambos; verificar sinal, polaridade, baud e referência GND; não prova interferência externa por si só |
| CRC ou bad_frames aumenta | Conferir parâmetros, sinal/eco e integridade do quadro |
| ignored aumenta | Endereço diferente de 1 ou quadros ignorados/eco; analisar antes de mudar o mapa |
| exceptions aumenta | Conferir função de leitura, faixa e quantidade; objetos/áreas automáticas fora do mapa |
| Número aparece, mas instrumento não está válido | Revisar os indicadores valid, unidade e perda do link da própria IHM |

Antes de entregar uma nova tela, confirmar: heartbeat atualizando; canais 247/246/245 sem troca; escala decimal correta; unidade correta; indicação de dado inválido; tratamento da comunicação perdida; navegação sem escrita Modbus; compilação sem erros. Não desligar sensor ou alimentação de processos ativos sem o procedimento da equipe.

## Mapa completo e controle de versão

O arquivo [holding-registers.csv](../screens/bench/holding-registers.csv) contém todas as 64 palavras, tipos, escalas e significados. Ele acompanha este guia e não exige acesso ao código-fonte. A tabela resumida acima contém os endereços necessários às telas de temperatura e diagnóstico.

Mudanças apenas de apresentação não exigem alterar o mapa. Para um dado/comando novo, solicitar uma extensão documentada: nome, unidade, tipo, acesso, condição de validade e comportamento de falha. O responsável pelo firmware deverá publicar a compatibilidade/versão antes de a equipe de telas usar os novos endereços. Não reaproveitar um endereço existente com outro significado.

### Referências

- [Pinagem e instalação oficial DOP-107BV](https://filecenter.deltaww.com/Products/download/06/060302/Manual/DELTA_IA-HMI_DOP-107BV_TC-SC-EN-TUR_20191016.pdf).
- [Endereçamento oficial Modbus 984 RTU/ASCII Master](https://filecenter.deltaww.com/Products/download/06/060302/Manual/DELTA_IA-HMI_Modbus%20984%20RTU-ASCII_Master_CM_EN_20130823.pdf).
- Contrato conferido na implementação atual e no mapa fornecido com esta revisão. A especificação do firmware prevalece para versões diferentes desta referência.

## Extensão ADC — firmware 0.3.0, mapa 2

Os endereços anteriores foram preservados. W40048…W40058 agora publicam ADC; somente offsets 58…63 continuam reservados. A especificação atual está em [README_ADC.md](../../../fw/testbench/MVP_H753_Hardware_Bench/Application/README_ADC.md) e no [CSV do mapa](../screens/bench/holding-registers.csv). Usar W40048 como condição de validade para W40052 (mV, dividir por 1000 para V). A leitura é tensão no pino PA6 com referência nominal, ainda sem validação elétrica. Não foi alterado o projeto nativo DOPSoft.
