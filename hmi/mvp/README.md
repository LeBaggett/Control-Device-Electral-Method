# IHM Delta DOP-107BV

Integração da IHM no MVP STM32H753. A bancada 0.3.0-adc-dma implementa escravo Modbus RTU pela UART4 em PD1/TX e PD0/RX, endereço 1, 9600/8N1, recepção DMA circular. Tela inicial somente leitura: três NOVUS, validade, unidade, Ethernet e heartbeat.

- [Seção DOPSoft e projeto editável](dopsoft/README.md): arquivo DPA, arquitetura STM32, comunicação, mapa Modbus e exemplos de objetos de temperatura/ADC.
- [Guia independente para criar telas](protocol/GUIA_TELAS_DOPSOFT.md): configuração DOPSoft, mapa, formatos, validade e diagnóstico, sem acessar o firmware.
- [Integração e limites](INTEGRACAO_UART4.md): firmware, pinos, diagnóstico e validação pendente.
- [Tela de bancada](screens/bench/README.md): fundo 800×480, objetos e mapa completo de registradores.
- [Artes originais](artwork/README.md): referência React/TypeScript fornecida, preservada.
- [Protocolo](protocol/README.md): responsabilidades do canal.

DOPSoft 4.00.16 instalado. Projeto nativo versionado em `dopsoft/CAIS_MVP_Bancada.dpa`, preservado como recebido. Os PNG/CSV/JSON são arte e especificação; a inclusão do DPA não comprova compilação ou finalização de todas as telas. A versão atual publica mapa 2 com ADC de diagnóstico, ainda pendente de validação elétrica. O usuário confirmou a comunicação UART4 em 9600/8N1 (1631 consultas/respostas, sem erros no diagnóstico enviado) e o heartbeat W40004 atualizando na IHM. O programa nativo permanece trabalho em andamento; novas telas ainda exigem revisão e validação própria. Os ensaios/calibração e comandos operacionais não fazem parte desta primeira tela de bancada.
