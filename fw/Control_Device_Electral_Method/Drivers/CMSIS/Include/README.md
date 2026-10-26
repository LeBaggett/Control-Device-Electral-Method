# Include - guia de manutenção

Dependência CMSIS/ARM/ST: núcleo, definições do MCU e templates de sistema. Não contém a lógica de ensaio da aplicação.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [cachel1_armv7.h](cachel1_armv7.h) | Interface C, tipos e constantes do módulo cachel1_armv7. Conferir includes e os consumidores antes de mudar assinaturas. |
| [cmsis_armcc.h](cmsis_armcc.h) | Interface C, tipos e constantes do módulo cmsis_armcc. Conferir includes e os consumidores antes de mudar assinaturas. |
| [cmsis_armclang.h](cmsis_armclang.h) | Interface C, tipos e constantes do módulo cmsis_armclang. Conferir includes e os consumidores antes de mudar assinaturas. |
| [cmsis_armclang_ltm.h](cmsis_armclang_ltm.h) | Interface C, tipos e constantes do módulo cmsis_armclang_ltm. Conferir includes e os consumidores antes de mudar assinaturas. |
| [cmsis_compiler.h](cmsis_compiler.h) | Interface C, tipos e constantes do módulo cmsis_compiler. Conferir includes e os consumidores antes de mudar assinaturas. |
| [cmsis_gcc.h](cmsis_gcc.h) | Interface C, tipos e constantes do módulo cmsis_gcc. Conferir includes e os consumidores antes de mudar assinaturas. |
| [cmsis_iccarm.h](cmsis_iccarm.h) | Interface C, tipos e constantes do módulo cmsis_iccarm. Conferir includes e os consumidores antes de mudar assinaturas. |
| [cmsis_version.h](cmsis_version.h) | Interface C, tipos e constantes do módulo cmsis_version. Conferir includes e os consumidores antes de mudar assinaturas. |
| [core_armv81mml.h](core_armv81mml.h) | CMSIS: acesso ao núcleo ARM e seus registradores/intrínsecos. Dependência de terceiro; preservar os termos de licença. |
| [core_armv8mbl.h](core_armv8mbl.h) | CMSIS: acesso ao núcleo ARM e seus registradores/intrínsecos. Dependência de terceiro; preservar os termos de licença. |
| [core_armv8mml.h](core_armv8mml.h) | CMSIS: acesso ao núcleo ARM e seus registradores/intrínsecos. Dependência de terceiro; preservar os termos de licença. |
| [core_cm0.h](core_cm0.h) | CMSIS: acesso ao núcleo ARM e seus registradores/intrínsecos. Dependência de terceiro; preservar os termos de licença. |
| [core_cm0plus.h](core_cm0plus.h) | CMSIS: acesso ao núcleo ARM e seus registradores/intrínsecos. Dependência de terceiro; preservar os termos de licença. |
| [core_cm1.h](core_cm1.h) | CMSIS: acesso ao núcleo ARM e seus registradores/intrínsecos. Dependência de terceiro; preservar os termos de licença. |
| [core_cm23.h](core_cm23.h) | CMSIS: acesso ao núcleo ARM e seus registradores/intrínsecos. Dependência de terceiro; preservar os termos de licença. |
| [core_cm3.h](core_cm3.h) | CMSIS: acesso ao núcleo ARM e seus registradores/intrínsecos. Dependência de terceiro; preservar os termos de licença. |
| [core_cm33.h](core_cm33.h) | CMSIS: acesso ao núcleo ARM e seus registradores/intrínsecos. Dependência de terceiro; preservar os termos de licença. |
| [core_cm35p.h](core_cm35p.h) | CMSIS: acesso ao núcleo ARM e seus registradores/intrínsecos. Dependência de terceiro; preservar os termos de licença. |
| [core_cm4.h](core_cm4.h) | CMSIS: acesso ao núcleo ARM e seus registradores/intrínsecos. Dependência de terceiro; preservar os termos de licença. |
| [core_cm55.h](core_cm55.h) | CMSIS: acesso ao núcleo ARM e seus registradores/intrínsecos. Dependência de terceiro; preservar os termos de licença. |
| [core_cm7.h](core_cm7.h) | CMSIS: acesso ao núcleo ARM e seus registradores/intrínsecos. Dependência de terceiro; preservar os termos de licença. |
| [core_cm85.h](core_cm85.h) | CMSIS: acesso ao núcleo ARM e seus registradores/intrínsecos. Dependência de terceiro; preservar os termos de licença. |
| [core_sc000.h](core_sc000.h) | CMSIS: acesso ao núcleo ARM e seus registradores/intrínsecos. Dependência de terceiro; preservar os termos de licença. |
| [core_sc300.h](core_sc300.h) | CMSIS: acesso ao núcleo ARM e seus registradores/intrínsecos. Dependência de terceiro; preservar os termos de licença. |
| [core_starmc1.h](core_starmc1.h) | CMSIS: acesso ao núcleo ARM e seus registradores/intrínsecos. Dependência de terceiro; preservar os termos de licença. |
| [mpu_armv7.h](mpu_armv7.h) | Interface C, tipos e constantes do módulo mpu_armv7. Conferir includes e os consumidores antes de mudar assinaturas. |
| [mpu_armv8.h](mpu_armv8.h) | Interface C, tipos e constantes do módulo mpu_armv8. Conferir includes e os consumidores antes de mudar assinaturas. |
| [pac_armv81.h](pac_armv81.h) | Interface C, tipos e constantes do módulo pac_armv81. Conferir includes e os consumidores antes de mudar assinaturas. |
| [pmu_armv8.h](pmu_armv8.h) | Interface C, tipos e constantes do módulo pmu_armv8. Conferir includes e os consumidores antes de mudar assinaturas. |
| [tz_context.h](tz_context.h) | Interface C, tipos e constantes do módulo tz_context. Conferir includes e os consumidores antes de mudar assinaturas. Entradas/símbolos para consulta: `TZ_InitContextSystem_S`, `TZ_AllocModuleContext_S`, `TZ_FreeModuleContext_S`, `TZ_LoadContext_S`, `TZ_StoreContext_S`. |

## Observações para manutenção

Conteúdo de terceiro: preserve a licença e a documentação upstream. Mudanças no pacote exigem revisar includes, configuração e compatibilidade das interfaces.

[Voltar à pasta superior](../README.md)
