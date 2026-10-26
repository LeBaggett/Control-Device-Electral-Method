# fw - guia de manutenção

Projetos embarcados: firmware oficial entregue e projetos separados de diagnóstico/referência.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Subpastas

| Pasta | Responsabilidade |
|---|---|
| [Control_Device_Electral_Method](Control_Device_Electral_Method/README.md) | Pasta integrante do firmware oficial entregue; use o inventário e os links abaixo para localizar interfaces, implementações e arquivos de suporte. |
| [testbench](testbench/README.md) | Referências e projetos isolados de teste. Conferir MCU, pinagem e bibliotecas de cada projeto antes de gravar; não compõem automaticamente o firmware principal. |

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| Sem arquivos diretos | Consulte os READMEs das subpastas acima. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
