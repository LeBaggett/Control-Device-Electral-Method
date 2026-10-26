# examples - guia de manutenção

Thermal Analyzer: aplicação Python/Tkinter para importar CAL/TST/TSP, visualizar medições e gerar relatórios; não controla a potência do dispositivo.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [CAL_001.CSV](CAL_001.CSV) | Registro tabular de exemplo ou aquisição. Conferir cabeçalho/metadados antes de escolher CAL/TST/TSP; preservar o original. |
| [CALIB.TXT](CALIB.TXT) | Arquivo de suporte/documentação/configuração CALIB.TXT; conferir as referências locais antes de editar. |
| [LEIA_PRIMEIRO.txt](LEIA_PRIMEIRO.txt) | Arquivo de suporte/documentação/configuração LEIA_PRIMEIRO.txt; conferir as referências locais antes de editar. |
| [SOFIA_DEMO.CSV](SOFIA_DEMO.CSV) | Registro tabular de exemplo ou aquisição. Conferir cabeçalho/metadados antes de escolher CAL/TST/TSP; preservar o original. |
| [TSP_DEMO.CSV](TSP_DEMO.CSV) | Registro tabular de exemplo ou aquisição. Conferir cabeçalho/metadados antes de escolher CAL/TST/TSP; preservar o original. |
| [TST_001.CSV](TST_001.CSV) | Registro tabular de exemplo ou aquisição. Conferir cabeçalho/metadados antes de escolher CAL/TST/TSP; preservar o original. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
