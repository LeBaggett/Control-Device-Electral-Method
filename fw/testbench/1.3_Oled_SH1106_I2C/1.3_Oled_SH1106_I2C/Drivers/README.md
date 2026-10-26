# Drivers - guia de manutenção

Bibliotecas de periféricos do fabricante. Manter fontes e headers da mesma versão; registrar qualquer ajuste local antes de atualizar o pacote.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Subpastas

| Pasta | Responsabilidade |
|---|---|
| [CMSIS](CMSIS/README.md) | Dependência CMSIS/ARM/ST: núcleo, definições do MCU e templates de sistema. Não contém a lógica de ensaio da aplicação. |
| [STM32F1xx_HAL_Driver](STM32F1xx_HAL_Driver/README.md) | Bibliotecas de periféricos do fabricante. Manter fontes e headers da mesma versão; registrar qualquer ajuste local antes de atualizar o pacote. |

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| Sem arquivos diretos | Consulte os READMEs das subpastas acima. |

## Observações para manutenção

Conteúdo de terceiro: preserve a licença e a documentação upstream. Mudanças no pacote exigem revisar includes, configuração e compatibilidade das interfaces.

[Voltar à pasta superior](../README.md)
