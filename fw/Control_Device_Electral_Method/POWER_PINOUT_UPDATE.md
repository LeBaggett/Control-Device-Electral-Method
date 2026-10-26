# Pinout de potência atualizado

Pedido do usuário: SELECT agora em PD12; RELAY agora em PD14.
Níveis preservados: PD12 HIGH=medição e LOW=aquecimento; PD14 LOW=desligado,
HIGH=ligado. Partida segura: SELECT em medição antes de relé desligado.

Atualizados Application/board/pinout.h, Core/Inc/main.h, Core/Src/main.c,
board.c e .ioc. Clock GPIOD habilitado antes de tocar nos latches de saída.
Nenhuma saída de potência deve ser conectada a PE0/PE1 nesta revisão de PCB.
O encoder continua PA15/PB3 + botão PE2; LED da bancada PA1 ativo baixo.

Faça Refresh, Clean e Build. Reabra o .ioc para conferir PD12/PD14 no CubeMX.
Não precisa gerar código novamente para aplicar estes arquivos; a configuração
.ioc foi alinhada para a próxima geração. Grave a versão correspondente ao PCB.
Testes de startup/ordem/intertravamentos e build ARM passaram; sem validação física.
