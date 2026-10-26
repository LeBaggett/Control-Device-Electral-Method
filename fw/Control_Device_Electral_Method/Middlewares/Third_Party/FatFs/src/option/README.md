# option - guia de manutenção

Middleware FatFs e suporte: implementação do sistema de arquivos. Configuração de placa está em FATFS/Target, não neste diretório.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [syscall.c](syscall.c) | Implementação C do módulo syscall. Conferir o header correspondente e as chamadas antes de alterar. Entradas/símbolos para consulta: `ff_cre_syncobj`, `ff_del_syncobj`, `ff_req_grant`, `ff_rel_grant`, `ff_memfree`. |

## Observações para manutenção

Conteúdo de terceiro: preserve a licença e a documentação upstream. Mudanças no pacote exigem revisar includes, configuração e compatibilidade das interfaces.

[Voltar à pasta superior](../README.md)
