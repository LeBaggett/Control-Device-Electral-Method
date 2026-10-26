# src - guia de manutenção

Middleware FatFs e suporte: implementação do sistema de arquivos. Configuração de placa está em FATFS/Target, não neste diretório.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Subpastas

| Pasta | Responsabilidade |
|---|---|
| [option](option/README.md) | Middleware FatFs e suporte: implementação do sistema de arquivos. Configuração de placa está em FATFS/Target, não neste diretório. |

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [diskio.c](diskio.c) | Despacho de operações de disco para o driver registrado. |
| [diskio.h](diskio.h) | Despacho de operações de disco para o driver registrado. |
| [ff.c](ff.c) | Implementação central do sistema de arquivos FatFs. Preservar versão e configuração compatíveis. Entradas/símbolos para consulta: `mem_cpy`, `mem_set`, `mem_cmp`, `chk_chr`, `lock_fs`, `unlock_fs`, `clear_lock`, `st_clust`, `cmp_lfn`, `pick_lfn`, `put_lfn`, `gen_numname`. |
| [ff.h](ff.h) | Implementação central do sistema de arquivos FatFs. Preservar versão e configuração compatíveis. Entradas/símbolos para consulta: `f_putc`, `f_puts`, `f_printf`, `ff_memfree`, `ff_cre_syncobj`, `ff_req_grant`, `ff_rel_grant`, `ff_del_syncobj`. |
| [ff_gen_drv.c](ff_gen_drv.c) | Registro e ligação dos drivers de disco FatFs ao volume lógico. Entradas/símbolos para consulta: `FATFS_LinkDriverEx`, `FATFS_LinkDriver`, `FATFS_UnLinkDriverEx`, `FATFS_UnLinkDriver`, `FATFS_GetAttachedDriversNbr`. |
| [ff_gen_drv.h](ff_gen_drv.h) | Registro e ligação dos drivers de disco FatFs ao volume lógico. Entradas/símbolos para consulta: `FATFS_LinkDriver`, `FATFS_UnLinkDriver`, `FATFS_LinkDriverEx`, `FATFS_UnLinkDriverEx`, `FATFS_GetAttachedDriversNbr`. |
| [ff_unicode.c](ff_unicode.c) | Conversão de caracteres usada por nomes de arquivos FatFs; necessária conforme ffconf.h. |
| [integer.h](integer.h) | Tipos inteiros de compatibilidade da versão FatFs integrada. |

## Observações para manutenção

Conteúdo de terceiro: preserve a licença e a documentação upstream. Mudanças no pacote exigem revisar includes, configuração e compatibilidade das interfaces.

[Voltar à pasta superior](../README.md)
