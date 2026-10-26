# thermal_report - guia de manutenção

Thermal Analyzer: aplicação Python/Tkinter para importar CAL/TST/TSP, visualizar medições e gerar relatórios; não controla a potência do dispositivo.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Fluxo e manutenção

`data` importa arquivos e modelos → `analysis` calcula/valida → `plots` desenha → `report` exporta. `gui` coordena seleção/condições sem alterar os dados originais.

- Preservar unidades: volts, °C/V, °C, segundos e microssegundos conforme o campo.
- CSVs não são modificados; guardar SHA-256, origem da temperatura, avisos e metadados na exportação.
- CAL selecionado explicitamente pode recalcular TST. TSP conserva o modelo registrado pelo protocolo.
- A razão `(Tj - Tcase)/P` usa o ponto HOT e exige declaração de regime permanente; temperatura de caixa não é temperatura ambiente.
- Arquivos parciais podem conter linhas válidas: não interpretar importação bem-sucedida como conclusão física do ensaio.
- Mudanças de cabeçalho/metadata exigem revisar leitor, exportador, exemplos e testes em conjunto.


## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [__init__.py](__init__.py) | Identificação/exportações do pacote Python. |
| [analysis.py](analysis.py) | Cálculos e validações dos ensaios CAL/TST/TSP; conserva a origem de Tj e condiciona razões térmicas aos dados e regime declarados. Entradas/símbolos para consulta: `validate`, `fit_calibration`, `analyze`, `analyze_sofia`, `value`. |
| [bootstrap.py](bootstrap.py) | Prepara o ambiente Tcl/Tk quando necessário ao iniciar a interface. Entradas/símbolos para consulta: `prepare_environment`. |
| [data.py](data.py) | Leitura dos CSVs/metadados e calibrações com CRC; modelos de dados, avisos e hash da origem. Não escreve nos arquivos de aquisição. Entradas/símbolos para consulta: `temperature`, `number`, `read_run`, `read_calibration`. |
| [gui.py](gui.py) | Interface Tkinter, seleção de registros/calibração, condições e acionamento da análise/exportação. Entradas/símbolos para consulta: `__init__`, `conditions`, `optional`, `rebuild_models`, `selected_model`, `import_paths`, `open_folder`, `open_files`, `open_model`, `clear`, `demo`, `refresh`. |
| [plots.py](plots.py) | Gráficos Matplotlib de calibração, ensaio por alvos e resfriamento TSP; conferir eixos, unidades e dados sintéticos. Entradas/símbolos para consulta: `make_figure`, `sofia_figure`. |
| [report.py](report.py) | Exporta PDF/HTML, gráficos, dados analisados e resumo estruturado em pasta nova. Entradas/símbolos para consulta: `fmt`, `export_bundle`, `_html`, `_pdf`, `table`, `footer`. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)
