# Modbus RTU

Parser, CRC, transações, timeout e mapeamento entre registradores e dados do dispositivo.

Estado: estrutura inicial criada em 26/10/2026.

Separar o STM32 mestre dos indicadores do STM32 escravo da IHM em UARTs/barramentos independentes. DMA RX/IDLE não substitui a delimitação RTU. DE deve ser liberado após UART Transmission Complete, não só após a conclusão DMA.

## Manutenção

Implementações futuras devem manter as responsabilidades desta camada e atualizar este README com os arquivos efetivamente criados. Arquivos de outras gerações não devem ser copiados sem revisão de interfaces e premissas.
