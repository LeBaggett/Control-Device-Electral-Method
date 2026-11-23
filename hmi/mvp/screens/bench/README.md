# Tela de bancada — Delta DOP-107BV / UART4

## Estado da entrega

Firmware 0.2.6-hmi-bench preparado para IHM mestre Modbus RTU, STM32 escravo endereço 1, 115200 baud, 8N1. PC10/TX e PC11/RX confirmados pelo usuário. O módulo RS485/TTL é separado do usado pelos NOVUS e tem direção automática. UART4 usa DMA1 Stream1; os NOVUS continuam em USART2/DMA1 Stream0. Ethernet e perfil rápido dos NOVUS são preservados.

**Ainda não existe arquivo de projeto DOPSoft nem programa transferido à IHM.** DOPSoft 4.00.16 instalado pelo usuário e aberto nesta etapa; projeto nativo ainda em montagem. A porta COM/DB9 física aguarda confirmação. Estes arquivos são uma arte final de fundo e uma especificação objetiva dos objetos nativos para montar o projeto; não substituem um .dop compilado. Não houve validação da UART4 ou da tela na IHM física.

## Arquivos

- bancada-background.png: fundo 800×480, sem valores fictícios; pronto para importar no DOPSoft.
- bancada-background.html: fonte do desenho, com o cabeçalho fornecido pelo usuário incorporado. Não precisa de internet para renderizar.
- objetos-dopsoft.csv e objetos-dopsoft.json: posições, dimensões, tipos, escalas, unidades e condições de visibilidade dos objetos a sobrepor à imagem. São especificações, não arquivos de importação automática garantida do DOPSoft.
- holding-registers.csv: mapa completo de 64 palavras somente leitura.

Os espaços em branco serão preenchidos por objetos do DOPSoft. A imagem isolada não recebe dados nem responde a toques. O cabeçalho e a identidade visual reutilizam o material entregue em ihmscreens.zip.

## Montagem da primeira tela no DOPSoft

1. Instalar DOPSoft 4 compatível com a DOP-107BV a partir da central oficial Delta. Link: https://www.deltaww.com/en-US/service-support/download-center?sectionId=2 . O modelo aparece no manual oficial DOPSoft: https://filecenter.deltaww.com/Products/download/06/060302/Manual/DELTA_IA-HMI_DOPSoft_UM_EN_20211230.pdf . A versão efetiva do instalador deve ser conferida antes da instalação.
2. Criar um projeto novo para DOP-107BV, 800×480. Salvar em hmi/mvp/dopsoft/ quando o editor estiver disponível. Não sobrescrever um programa existente da IHM sem preservar a cópia que a equipe precisa.
3. Configurar a porta COM que está fisicamente ligada ao adaptador DB9. Selecionar RS485 de dois fios, driver Modbus RTU mestre, estação/escravo STM=1, 115200 baud, 8 bits, sem paridade, um stop bit, timeout inicial 500 ms. Começar com leitura a cada 250–500 ms.
4. O STM suporta leitura de holding registers (FC03) e também FC04 para leitura do mesmo mapa nesta bancada. Não selecionar Delta DVP, Modbus ASCII ou protocolo de PLC diferente. Não colocar os NOVUS no barramento da IHM: eles permanecem no outro canal.
5. Inserir o fundo PNG em 0,0, com largura 800 e altura 480, sem redimensionamento adicional. Travar o fundo para não mover ao posicionar objetos.
6. Validar primeiro o registrador de offset 0: ele deve ler 0xCA15. A notação documental é 40001. Alguns drivers mostram 4x e base 1, outros usam offset 0; conferir a convenção do driver escolhido, sem enviar literalmente 40001 como endereço de protocolo. Se houver exceção de endereço, revisar essa base antes de mudar o firmware.
7. Adicionar Numeric Display para as temperaturas: offsets 11/21/31, correspondentes a 40012/40022/40032 na notação 4xxxx base 1. Tipo signed 16-bit, uma casa decimal. Ex.: 266 representa 26,6 graus; não multiplicar novamente por 10.
8. Cada temperatura precisa do seu indicador de validade: offsets 12/22/32. Mostrar o número apenas quando válido. Adicionar unidade multestado offsets 19/29/39 (0=°C; 1=°F); não rotular um dado em °F como °C.
9. Adicionar Ethernet no offset 46, quantidade válida no 6 e heartbeat/tempo ligado no 3, conforme objetos-dopsoft.csv. A leitura de Ethernet não representa o estado da comunicação da IHM.
10. Habilitar a indicação de erro de comunicação do próprio DOPSoft/IHM e ocultar/sinalizar números retidos quando o link serial falhar. O valid recebido do STM não se atualiza se a própria IHM deixar de comunicar; não usar somente esse bit para indicar conexão viva.
11. Compilar no editor e testar a leitura do heartbeat; depois comparar os três valores com a página web e os displays. A transferência para a IHM exige o projeto nativo compilado e conexão de programação reconhecida pelo DOPSoft. Ainda não foi executada nesta entrega.

## Estratégia e diagnóstico

A Delta é mestre neste canal: ela inicia os pedidos; o STM só responde. Não enviar comandos espontâneos da UART4 nem tentar executar JavaScript na Delta. A tela é somente leitura, sem botões de atuação. As consultas dos NOVUS continuam independentes e fornecem snapshots para a IHM.

A página Ethernet ganha `/api/hmi` e o painel IHM/UART4. Também há `g_hmi_debug` e `g_hmi_uart_debug` em Live Expressions. Sem projeto configurado na Delta, zero pedidos na UART4 é esperado. Se a IHM consultar e os bytes ficarem zerados, conferir fiação/pinos/porta; se bytes chegarem mas CRC aumentar, conferir parâmetros e integridade; se ignored aumentar, conferir endereço e eco. Requests/responses crescentes não provam sozinhos atualização correta da tela: verificar heartbeat, unidade e validade.

Mapa: palavras de 16 bits, bytes mais significativos primeiro em cada palavra, temperaturas assinadas em complemento de dois, sem troca de palavras de float. Contadores exportados como palavras se repetem após 65536; para contadores completos usar o JSON. Requisições broadcast ou a outros endereços não recebem resposta. Escritas recebem exceção de função inválida. Pedidos fora do mapa ou com quantidade inválida recebem exceção; CRC inválido é descartado sem resposta.

O recebimento usa DMA circular e silêncio de frame no superloop, sem processamento Modbus em ISR. A nova UART4 não habilita fonte, relé, DAC ou aquecedor. ADC/DAC permanecem fora desta etapa. Para desabilitar a integração da IHM, definir BENCH_HMI_ENABLED=0 em Application/bench_config.h, recompilar e gravar; o ponto anterior é 63759a5.

## Validação física pendente

UART4 no hardware, direção do módulo, porta COM/DB9, driver/projeto DOPSoft, correspondência do mapa, temperaturas negativas, falha de sensor, desligamento/reconexão da IHM e operação conjunta com Ethernet/NOVUS. O firmware foi testado no computador com transporte simulado; isso não substitui os testes físicos.
