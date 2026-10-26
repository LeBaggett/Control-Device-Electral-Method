# testbench - guia de manutenção

Referências e projetos isolados de teste. Conferir MCU, pinagem e bibliotecas de cada projeto antes de gravar; não compõem automaticamente o firmware principal.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Subpastas

| Pasta | Responsabilidade |
|---|---|
| [1.3_Oled_SH1106_I2C](1.3_Oled_SH1106_I2C/README.md) | Projeto de referência do OLED/SH1106, com arquivos STM32F1. Não substituir diretamente a pinagem/HAL do projeto F407. |
| [Driver_Hardware_Bench](Driver_Hardware_Bench/README.md) | Projeto manual de validação de drivers e comunicação. O estado local inclui diagnóstico SD somente de leitura e pode divergir da versão oficial entregue. |
| [OLED_Only_F407](OLED_Only_F407/README.md) | Projeto isolado de diagnóstico do OLED no STM32F407. Não executa o ensaio térmico completo. |
| [Rotary-Encoder-with-STM32](Rotary-Encoder-with-STM32/README.md) | Referências e projetos isolados de teste. Conferir MCU, pinagem e bibliotecas de cada projeto antes de gravar; não compõem automaticamente o firmware principal. |
| [STM32-MAX31855](STM32-MAX31855/README.md) | Referências e projetos isolados de teste. Conferir MCU, pinagem e bibliotecas de cada projeto antes de gravar; não compõem automaticamente o firmware principal. |

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| Sem arquivos diretos | Consulte os READMEs das subpastas acima. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
