# Legacy - guia de manutenção

Bibliotecas de periféricos do fabricante. Manter fontes e headers da mesma versão; registrar qualquer ajuste local antes de atualizar o pacote.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [stm32_hal_legacy.h](stm32_hal_legacy.h) | Interface C, tipos e constantes do módulo stm32_hal_legacy. Conferir includes e os consumidores antes de mudar assinaturas. |

## Observações para manutenção

Conteúdo de terceiro: preserve a licença e a documentação upstream. Mudanças no pacote exigem revisar includes, configuração e compatibilidade das interfaces.

[Voltar à pasta superior](../README.md)
