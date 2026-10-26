# Firmware oficial do MVP - STM32H753

Projeto novo para NUCLEO-H753ZI. Mantém as camadas da POC, com periféricos e comunicação próprios do H7. Nesta etapa há estrutura e contratos documentais; não há projeto CubeMX gerado nem firmware H753 compilável.

Estado: estrutura inicial criada em 26/10/2026.

## Ordem de inicialização prevista

Estado seguro dos atuadores → clocks/memória/cache → referência de tempo → aquisição → comunicação serial/Ethernet → aplicação. A ordem final será validada com o hardware; nenhum comando de rede deve energizar o DUT no boot.

## Integração futura com CubeMX

Selecionar a placa/revisão NUCLEO-H753ZI correta e gerar o `.ioc` nesta pasta. Conferir a geração para Core, Drivers, Middlewares e LWIP sem substituir arquivos da POC. Não reutilizar linker, startup, pinagem ou HAL F407 como se fossem H753.

O ponto de entrada futuro será `Core/Src/main.c`; `Application/app.c` coordenará as sessões. Pinos serão centralizados em `Application/board/pinout.h`, ainda não criado por falta de definição da pinagem.

## Subpastas

- [Application](Application/README.md)
- [Core](Core/README.md)
- [Drivers](Drivers/README.md)
- [LWIP](LWIP/README.md)
- [Middlewares](Middlewares/README.md)

## Manutenção

Implementações futuras devem manter as responsabilidades desta camada e atualizar este README com os arquivos efetivamente criados. Arquivos de outras gerações não devem ser copiados sem revisão de interfaces e premissas.
