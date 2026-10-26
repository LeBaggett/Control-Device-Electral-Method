# thermal_analyzer - guia de manutenção

Thermal Analyzer: aplicação Python/Tkinter para importar CAL/TST/TSP, visualizar medições e gerar relatórios; não controla a potência do dispositivo.

Documentação revisada em 26/10/2026. Os comentários e guias não alteram a lógica executável.

## Uso

Requer Python 3.11+ com Tcl/Tk. Executar `INICIAR.bat`; o inicializador prepara `.venv` e instala `requirements.txt` quando necessário. A instalação inicial pode requerer internet.

Importar pasta SD ou CSVs individuais → conferir tipo/pontos/avisos → selecionar origem de Tj → informar condições reais → exportar relatório. Exportação produz PDF/HTML, gráficos, CSV analisado e JSON em pasta nova; copiar os brutos separadamente.

Após demonstração, revisar a marcação de dados sintéticos e as condições: limpar a lista não reinicializa todos os campos. Temperatura de caixa, junção e ambiente são grandezas distintas.


## Subpastas

| Pasta | Responsabilidade |
|---|---|
| [examples](examples/README.md) | Thermal Analyzer: aplicação Python/Tkinter para importar CAL/TST/TSP, visualizar medições e gerar relatórios; não controla a potência do dispositivo. |
| [tests](tests/README.md) | Thermal Analyzer: aplicação Python/Tkinter para importar CAL/TST/TSP, visualizar medições e gerar relatórios; não controla a potência do dispositivo. |
| [thermal_report](thermal_report/README.md) | Thermal Analyzer: aplicação Python/Tkinter para importar CAL/TST/TSP, visualizar medições e gerar relatórios; não controla a potência do dispositivo. |

## Arquivos e implementações

| Arquivo | Responsabilidade e manutenção |
|---|---|
| [.gitignore](.gitignore) | Arquivo de suporte .gitignore; identificar seu consumidor antes de modificar ou remover. |
| [INICIAR.bat](INICIAR.bat) | Script de operação/ambiente. Ler os comandos antes de executar e confirmar os caminhos utilizados. |
| [main.py](main.py) | Entrada do programa no computador: prepara o ambiente e inicia a interface Tkinter. Não é o main.c do STM32. |
| [requirements.txt](requirements.txt) | Dependências Python e faixas de versão do Thermal Analyzer; instalar no ambiente virtual da aplicação. |

## Observações para manutenção

Antes de alterar interfaces, procure seus consumidores e testes. Após mudanças, revise o diff e compile o projeto correspondente; confirme em bancada quando houver efeito elétrico ou temporal.

[Voltar à pasta superior](../README.md)


## Documentação anterior preservada

O conteúdo abaixo registra etapas anteriores. Em caso de divergência, conferir o guia atual acima e os fontes da versão correspondente.

<details>
<summary>Abrir documentação anterior</summary>

# Analisador de caracterização térmica

Aplicativo Python com Tkinter. Importa os CSVs do firmware, mostra gráficos e gera um pacote de relatório. Não controla a placa nem altera os arquivos do SD.

## Iniciar no Windows

Instale Python 3.11 ou superior com **pip e Tcl/Tk**. Dê dois cliques em `INICIAR.bat`: na primeira execução ele cria um ambiente `.venv` e instala Matplotlib e ReportLab. É necessário acesso à internet nessa primeira instalação. As dependências ficam nessa pasta, sem mudar o Python global.

Alternativa pelo terminal:

```text
python -m venv .venv
.venv\Scripts\python.exe -m pip install -r requirements.txt
.venv\Scripts\python.exe main.py
```

## Usar

1. Conecte o cartão ao PC ou copie os registros para uma pasta local.
2. Clique em **Importar pasta do SD** ou **Adicionar CSVs**. A importação usa os arquivos na pasta selecionada, sem varrer subpastas.
3. Selecione um registro para ver gráficos, resultados e dados. Ctrl/clique permite selecionar vários arquivos para o mesmo relatório; a prévia mostra o primeiro selecionado.
4. Para ensaios, o padrão é usar **tj_c gravada no SD**. Opcionalmente selecione uma calibração CAL importada ou carregue `CALIB.TXT`/`CALIB.BAK` com CRC válido. A associação é explícita: o programa não assume que o último CAL pertence ao TST.
5. Preencha DUT, responsável e observações. Ambiente e potência são opcionais e devem representar temperatura ambiente e **potência efetivamente dissipada no DUT**, não apenas a tensão ou potência nominal da fonte. Use **Atualizar análise** após mudar os campos.
6. Rθj-a é estimada somente com ambiente, potência e a caixa de confirmação de regime permanente no **último ponto**. Essa caixa é uma declaração do usuário, não uma conclusão automática. Para relatórios com vários ensaios, essas condições serão aplicadas a todos os selecionados.
7. Clique em **Exportar relatório** e escolha uma pasta de saída. O programa cria uma subpasta nova com PDF, HTML independente, gráficos PNG/SVG, `dados_analisados.csv` e `resumo.json`. O JSON inclui hashes SHA-256, calibração usada, avisos e condições.

O botão **Demonstração** importa dados sintéticos e marca o relatório como demonstração. Não use esses exemplos como evidência de validação do hardware. Os campos de identificação ficam na aba lateral **Condições**.

Se o Python instalado trouxer os scripts Tcl/Tk em ZIP, o inicializador prepara uma cópia desses scripts na pasta temporária do usuário. Ele reutiliza somente a própria instalação local e não baixa nem modifica o Python global. Se Tcl/Tk não estiver instalado, use a opção de instalação/reparo do instalador oficial do Python.

Testado neste PC com Python 3.14.7, Tcl/Tk 9.0, Matplotlib 3.11.2 e ReportLab 5.0.1. Não é um executável independente de Python.

## Formatos do firmware

Calibração:

```csv
idx,t_alvo_c,t_rtd_c,vf_v
0,25.00,25.10,0.65000
```

Ensaio:

```csv
idx,t_alvo_c,t_rtd_c,vf_v,tj_c,t_s
0,30.00,30.12,0.63700,31.50,12.350
```

`t_rtd_c` é um nome legado: neste projeto representa o termopar/MAX31855. Não é aplicada uma segunda correção de termopar aos CSVs. `NaN` em Tj permanece ausente, nunca vira zero. Cabeçalhos diferentes são rejeitados; linhas incompletas são ignoradas e identificadas no relatório. Há suporte a UTF-8/BOM e a CSV com ponto e vírgula e vírgula decimal.

`CALIB.TXT` e `CALIB.BAK` usam o CRC32 do firmware sobre os bytes originais, inclusive quebras CRLF. Abrir/salvar o arquivo em um editor pode invalidar o CRC.

## Cálculos e limites

- Regressão igual à orientação do firmware: **T = m · Vf + t0**. Mostra m, t0, R², dVf/dT, resíduos e RMSE. O ajuste do próprio CAL não é uma validação independente; dois pontos com R²=1 não comprovam linearidade.
- Mostra pontos de temperatura e Vf ao longo do tempo/índice e diferença Tj-termopar. Essa diferença pode ser física, pois as temperaturas se referem a locais diferentes.
- **Rθj-a = (Tj final − Ta) / P**, somente nas condições declaradas acima. A estimativa depende das condições e dos dados informados.
- Os arquivos atuais são amostras por alvo. Não registram uma curva contínua, potência ao longo do tempo, corrente de medição, todas as condições de contorno ou comprovante da janela de 30 µs. Portanto, o programa não inventa Zth, constante de tempo, estado estacionário ou incertezas.
- O firmware não grava um identificador da calibração em cada TST. Ao recalcular Tj no PC, confirme por conta própria que a calibração pertence àquele DUT e às mesmas condições elétricas.

## Manutenção

`thermal_report/data.py`: leitura e CRC; `analysis.py`: fórmulas; `plots.py`: gráficos; `report.py`: PDF/HTML/dados; `gui.py`: interface. O código não depende de pandas, Qt ou de serviços online.

Testes:

```text
.venv\Scripts\python.exe -m unittest discover -s tests -v
```

A implementação usa os componentes de interface e gráficos conforme a [documentação de integração do Matplotlib com Tk](https://matplotlib.org/stable/gallery/user_interfaces/embedding_in_tk_sgskip.html). Exemplos e relatórios de demonstração são sintéticos. Nenhuma validação física do aplicativo com um ensaio real foi alegada.


## Novo ensaio TSP (v1.1)
Os CSV TST com #mode=TSP_STEP são identificados automaticamente. Usam as correntes fixas do dispositivo: 758 mA aquecimento e 2 mA medição, não editáveis na interface. A potência vem de 0,758×Vh medida durante aquecimento; não é substituída pelo campo de potência do formulário.
Mantenha Usar tj_c gravada no SD: a reta M/B específica do ensaio está no arquivo. Outra calibração selecionada no PC não substitui essa reta. O PDF/HTML/JSON contém o protocolo, correntes, modelo, atrasos, potência, Tj quente e limites; os gráficos mostram os instantes de resfriamento observados, em escala logarítmica. O CSV analisado mantém fase e tempo após SELECT.
MAX31855 mede a caixa. Informe Ta separadamente para Rθj-a e declare regime permanente apenas se confirmado antes do corte. A razão (Tj-Tcase)/P descreve o arranjo medido. Também não calcula thetaJB sem sensor de placa.
O exemplo TSP_DEMO.CSV é sintético. Consulte TSP_ENSAIO.md na pasta do firmware para executar o teste na placa.

</details>
