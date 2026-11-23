# Preparação da IHM Delta DOP-107BV / UART4

## Informação confirmada

O usuário informou uma IHM DOP-107BV conectada ao STM32 por UART4, usando a cadeia IHM → adaptador DB9/bornes → módulo RS485/TTL (mesmo tipo automático dos NOVUS) → STM32. O adaptador DB9 expõe contatos; o módulo separado converte o sinal. Esse canal é independente do USART2 dos três NOVUS.

## O que falta confirmar antes de inicializar

- Quais pinos físicos da NUCLEO estão ligados ao TX e RX da UART4.
- Qual porta COM da Delta foi usada e quais contatos DB9 estão ligados ao módulo. O desenho do conector deve ser lido pelo lado indicado no manual; não espelhar a numeração por observar o lado oposto.
- Porta da IHM em RS485 de dois fios, driver Modbus RTU mestre, baud/paridade/stop bits e endereço do STM32 escravo.
- Projeto DOPSoft e mapa dos objetos. O ZIP fornecido contém artes, fontes e exemplos PNG, não um projeto DOPSoft.

UART4 não pode ser inicializada apenas pelo nome do periférico: é necessário confirmar o mapeamento de pinos e conflitos. Por exemplo, PA1 já é usado pelo RMII da Ethernet nesta bancada; não pode ser reaproveitado para UART4 simultaneamente.

## Como começar no DOPSoft

1. Confirmar a versão do DOPSoft compatível com a DOP-107BV e criar/abrir um projeto para esse modelo, resolução 800×480.
2. Configurar a porta realmente cabeada como RS485 de dois fios e o driver Modbus RTU mestre apropriado.
3. Definir endereço/baud/paridade em conjunto com o firmware UART4. Ainda não foi atribuído um endereço ao STM32 neste canal; 247/246/245 pertencem aos indicadores no outro barramento.
4. Importar um PNG como fundo. Sobrepor objetos nativos para as três temperaturas, validade e comunicação. Botões e entradas precisam de objetos funcionais, não apenas desenhos.
5. Validar primeiro leitura de um registrador conhecido e heartbeat antes de adicionar comandos. O STM não tem ainda driver escravo UART4 nem mapa publicado: não esperar comunicação apenas por conectar os fios ou importar a imagem.
6. Após isso, integrar calibração e TSP às mesmas máquinas de estado usadas pelo firmware. Aquisição e segurança permanecem no STM.

O diagrama planejado é IHM mestre → canal RS485 dedicado → STM escravo, enquanto STM mestre → outro canal RS485 → três NOVUS escravos. A próxima implementação depende da confirmação física acima.
