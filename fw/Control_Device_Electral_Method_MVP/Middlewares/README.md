# Middleware embarcado

Destino de dependências geradas, especialmente a pilha Ethernet.

Estado: estrutura inicial criada em 26/10/2026.

Decisão: sem RTOS. A integração LwIP será em modo NO_SYS, atendida pelo superloop cooperativo. Não adicionar FreeRTOS/CMSIS-RTOS ao projeto.

## Subpastas

- [Third_Party](Third_Party/README.md)

## Manutenção

Implementações futuras devem manter as responsabilidades desta camada e atualizar este README com os arquivos efetivamente criados. Arquivos de outras gerações não devem ser copiados sem revisão de interfaces e premissas.
