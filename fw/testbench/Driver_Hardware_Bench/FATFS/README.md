# FATFS - guia de manutenção

Projeto manual de validação de drivers e comunicação. O estado local inclui diagnóstico SD somente de leitura e pode divergir da versão oficial entregue.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Subpastas

| Pasta | Responsabilidade |
|---|---|
| [App](App/README.md) | Projeto manual de validação de drivers e comunicação. O estado local inclui diagnóstico SD somente de leitura e pode divergir da versão oficial entregue. |
| [Target](Target/README.md) | Projeto manual de validação de drivers e comunicação. O estado local inclui diagnóstico SD somente de leitura e pode divergir da versão oficial entregue. |

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| Sem arquivos diretos | Consulte os READMEs das subpastas acima. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
