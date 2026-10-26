# Drivers da aplicação

Serviços de temperatura, aquisição elétrica, fonte de corrente, aquecedor, persistência e comunicação. Exportam interfaces para a aplicação sem desenhar telas.

Estado: estrutura inicial criada em 26/10/2026.

O N1040i mede temperatura e comunica por RS-485; a captura rápida de Vf pertence ao ADC/timer local. A fonte e o aquecedor ainda serão definidos. Drivers de potência devem oferecer retorno seguro e falhas explícitas.

## Subpastas

- [modbus](modbus/README.md)

## Manutenção

Implementações futuras devem manter as responsabilidades desta camada e atualizar este README com os arquivos efetivamente criados. Arquivos de outras gerações não devem ser copiados sem revisão de interfaces e premissas.
