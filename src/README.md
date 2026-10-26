# src - guia de manutenção

Manuais PDF arquivados. Apesar do nome src, esta pasta contém documentação, não as fontes ativas do MCU.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [Manual_Dispositivo_CAIS_20261018.pdf](Manual_Dispositivo_CAIS_20261018.pdf) | Manual/relatório PDF arquivado. Conferir a data e a versão; documentos antigos podem não refletir alterações posteriores do código. |
| [Manual_Thermal_Analyzer_CAIS_20261018.pdf](Manual_Thermal_Analyzer_CAIS_20261018.pdf) | Manual/relatório PDF arquivado. Conferir a data e a versão; documentos antigos podem não refletir alterações posteriores do código. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
