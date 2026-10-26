# Include - guia de manutenção

Dependência CMSIS/ARM/ST: núcleo, definições do MCU e templates de sistema. Não contém a lógica de ensaio da aplicação.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [stm32f103xb.h](stm32f103xb.h) | Interface C, tipos e constantes do módulo stm32f103xb. Conferir includes e os consumidores antes de mudar assinaturas. |
| [stm32f1xx.h](stm32f1xx.h) | Interface C, tipos e constantes do módulo stm32f1xx. Conferir includes e os consumidores antes de mudar assinaturas. |
| [system_stm32f1xx.h](system_stm32f1xx.h) | Interface C, tipos e constantes do módulo system_stm32f1xx. Conferir includes e os consumidores antes de mudar assinaturas. |

## Observações para manutenção

Conteúdo de terceiro: preserve a licença e a documentação upstream. Mudanças no pacote exigem revisar includes, configuração e compatibilidade das interfaces.

[Voltar à pasta superior](../README.md)
