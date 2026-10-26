# Control-Device-Electral-Method - guia de manutenção

Raiz do repositório do dispositivo de caracterização térmica e das ferramentas de análise.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Organização e versão de referência

O firmware oficial está em [fw/Control_Device_Electral_Method](fw/Control_Device_Electral_Method/README.md), baseado na versão entregue importada no commit `aff198f`. O responsável informou funcionamento em hardware; a manutenção documental verifica fontes/compilação sem repetir essa validação física.

| Área | Conteúdo |
|---|---|
| `fw/Control_Device_Electral_Method` | Projeto CubeIDE/CubeMX oficial e seus fontes. |
| `fw/testbench` | Bancadas e referências independentes, com versões e MCUs distintos. |
| `tools/thermal_analyzer` | Aplicação Python para CSVs, gráficos e relatórios. |
| `src` | Manuais PDF arquivados. |

## Onde alterar

Pinagem/controlador OLED: `Application/board/pinout.h`. Integração e ajustes de hardware: `Application/board/runtime_config.h`. Limites gerais: `Application/config.h`. Protocolos: `Application/core`. Telas: `Application/ui`. Persistência: `Application/drivers/sdlog.c` e FATFS. Análise no computador: `tools/thermal_analyzer/thermal_report`.

## Preservação da versão entregue

Não copiar automaticamente diagnósticos do bench para o principal. Antes de mudar hardware ou protocolo, conferir diferenças de fontes, `.ioc`, HAL e FatFs. Calibração depende do DUT, da corrente de medição e da montagem. Corrente nominal de 2 mA ajustável por trimpot interno; o firmware não regula essa corrente.

## Pastas de ambiente e saídas

`.metadata`, `.settings`, Debug/Release/Build, ambientes virtuais, caches, relatórios gerados e `__MACOSX` são configurações locais/saídas ou metadados. Não são a fonte do protocolo. Não editar executáveis/objetos para corrigir firmware; recompilar a fonte correspondente. Os índices abaixo cobrem as pastas de código, documentação e dependências.

## Subpastas

| Pasta | Responsabilidade |
|---|---|
| [fw](fw/README.md) | Projetos embarcados: firmware oficial entregue e projetos separados de diagnóstico/referência. |
| [src](src/README.md) | Manuais PDF arquivados. Apesar do nome src, esta pasta contém documentação, não as fontes ativas do MCU. |
| [tools](tools/README.md) | Ferramentas executadas no computador, separadas da imagem gravada no STM32. |

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [.gitmodules](.gitmodules) | Arquivo de suporte .gitmodules; identificar seu consumidor antes de modificar ou remover. |
| [LICENSE](LICENSE) | Licença aplicável aos arquivos distribuídos nesta pasta. Preservar atribuições e condições. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.
