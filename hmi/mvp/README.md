# IHM Delta DOP-107BV

Destino do projeto DOPSoft, telas e mapa de comunicação com o STM32.

Estado: estrutura inicial criada em 26/10/2026.

A IHM possui interfaces seriais; não presumir Ethernet neste modelo. Criar telas de operação, três temperaturas, calibração, TSP, estados e alarmes. Confirmar driver/protocolo no DOPSoft antes de definir registradores finais.

## Subpastas

- [protocol](protocol/README.md)
- [screens](screens/README.md)

## Manutenção

Implementações futuras devem manter as responsabilidades desta camada e atualizar este README com os arquivos efetivamente criados. Arquivos de outras gerações não devem ser copiados sem revisão de interfaces e premissas.
