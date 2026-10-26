# docs - guia de manutenção

Notas técnicas de integração e manutenção. Conferir o código atual quando houver diferenças entre documentos de etapas anteriores.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [BANCADA.md](BANCADA.md) | Documento específico: bancada. Conferir a data/contexto e confrontar afirmações históricas com o código entregue. |
| [CONTROLE_POTENCIA.md](CONTROLE_POTENCIA.md) | Documento específico: controle potencia. Conferir a data/contexto e confrontar afirmações históricas com o código entregue. |
| [PROTOCOLO.md](PROTOCOLO.md) | Documento específico: protocolo. Conferir a data/contexto e confrontar afirmações históricas com o código entregue. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
