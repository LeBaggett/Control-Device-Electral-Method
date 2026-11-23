# IHM Delta DOP-107BV

Destino do projeto DOPSoft, telas e mapa de comunicação com o STM32.

Estado em 2026-11-23: UART4 informada pelo usuário para a IHM; integração de firmware ainda não habilitada. Artes React/TypeScript recebidas e preservadas em artwork/reference/. Não há projeto DOPSoft no ZIP.

A IHM possui interfaces seriais; não presumir Ethernet neste modelo. Criar telas de operação, três temperaturas, calibração, TSP, estados e alarmes. Confirmar driver/protocolo no DOPSoft antes de definir registradores finais.

## Subpastas

- [protocol](protocol/README.md)
- [screens](screens/README.md)
- [artwork](artwork/README.md)
- [Integração inicial](INTEGRACAO_UART4.md)

## Manutenção

Implementações futuras devem manter as responsabilidades desta camada e atualizar este README com os arquivos efetivamente criados. Arquivos de outras gerações não devem ser copiados sem revisão de interfaces e premissas.
