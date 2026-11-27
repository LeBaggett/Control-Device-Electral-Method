# Arquivos locais incluídos no repositório

Em 2026-11-27 foram incluídos, a pedido do usuário, arquivos que antes permaneciam somente no computador de trabalho. O [inventário SHA-256](arquivos-locais-20261127.json) registra os bytes recebidos, antes de qualquer edição futura desses arquivos.

| Grupo | Finalidade |
|---|---|
| `Driver_Hardware_Bench/.settings/` | Preferências locais e indexação do CubeIDE do teste F407 |
| `MVP_H753_Hardware_Bench/.settings/` | Preferências locais e indexação do CubeIDE do MVP H753 |
| `OLED_Only_F407/.settings/` | Preferências locais e indexação do teste isolado do OLED |
| `MVP_H753_Hardware_Bench/MVP_H753_Hardware_Bench.launch` | Configuração de depuração existente, preservada |
| `OLED_Only_F407/firmware/*.bin`, `*.elf`, `*.map` | Binários preexistentes do teste OLED; não reconstruídos nesta inclusão |
| `../../hmi/mvp/dopsoft/CAIS_MVP_Bancada.dpa` | Projeto nativo da IHM, preservado como recebido |

As pastas `.settings` incluem `language.settings.xml` (indexação), `stm32cubeide.project.prefs` (preferências do projeto) e `com.st.stm32cube.ide.mcu.sfrview.prefs` (estado da visualização de registradores).

## Uso em outro computador

O arquivo `.launch` contém caminho de log do computador original e seleção de um número de série ST-LINK específico. Conferir esses dois campos em Debug Configurations ao usar outra máquina ou placa. As preferências do editor podem ser regeneradas pelo CubeIDE. O armazenamento desses arquivos não substitui instalar a toolchain nem compilar o projeto.

O conteúdo do DPA não foi alterado nem recompilado durante a organização. O hash confirma sua preservação; não confirma os endereços configurados dentro dele. Consultar o [guia DOPSoft](../../hmi/mvp/dopsoft/README.md) antes de acrescentar telas. A implementação do firmware permanece a do commit ADC anterior.
