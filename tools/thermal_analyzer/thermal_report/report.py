"""Exporta um pacote independente. Nunca escreve nos arquivos de entrada."""
from __future__ import annotations
import base64
import csv
from dataclasses import asdict
from datetime import datetime
from html import escape
from io import BytesIO
import json
from pathlib import Path
from . import __version__
from .analysis import Analysis, Conditions
from .plots import make_figure


def fmt(value):
    return "—" if value is None else f"{value:.6g}" if isinstance(value, (float, int)) else str(value)


def export_bundle(folder: Path, results: list[Analysis], conditions: Conditions) -> Path:
    if not results:
        raise ValueError("Selecione ao menos um arquivo para exportar.")
    conditions.validate()
    folder = Path(folder)
    # Evita a gravação de relatório sobre o próprio SD/CSV por engano.
    if any(folder.resolve() == r.run.path.parent.resolve() for r in results):
        raise ValueError("Use uma subpasta nova para o relatório.")
    folder.mkdir(parents=True, exist_ok=False)
    graphs = []
    for index, result in enumerate(results, 1):
        name = f"grafico_{index:02d}"
        fig = make_figure(result)
        fig.savefig(folder / (name + ".png"), dpi=180)
        fig.savefig(folder / (name + ".svg"))
        graphs.append((name, (folder / (name + ".png")).read_bytes()))
        fig.clear()
    metadata = {
        "app_version": __version__, "created": datetime.now().astimezone().isoformat(),
        "conditions": asdict(conditions), "runs": [
            {"file": str(r.run.path), "sha256": r.run.sha256, "kind": r.run.kind,
             "samples": len(r.run.samples), "skipped": r.run.skipped,
             "temperature_source": r.temperature_source, "metrics": r.metrics, "protocol_metadata": r.run.metadata,
             "calibration": asdict(r.calibration) if r.calibration else None,
             "warnings": r.warnings} for r in results],
    }
    (folder / "resumo.json").write_text(json.dumps(metadata, ensure_ascii=False, indent=2, allow_nan=False), encoding="utf-8")
    with (folder / "dados_analisados.csv").open("w", newline="", encoding="utf-8-sig") as f:
        writer = csv.writer(f)
        writer.writerow(["arquivo", "tipo", "idx", "t_alvo_c", "t_termopar_c", "vf_v", "tj_gravada_c", "tj_analisada_c", "t_s", "fase", "tempo_desde_select_us"])
        for r in results:
            safe_name = r.run.path.name
            if safe_name.startswith(("=", "+", "-", "@")):
                safe_name = "'" + safe_name
            for point, tj in zip(r.run.samples, r.junction):
                writer.writerow([safe_name, r.run.kind, point.idx, point.target, point.reference, point.vf,
                                 point.junction, tj, point.seconds, r.run.metadata.get(f"phase_{point.idx}",""), r.run.metadata.get(f"cut_us_{point.idx}","")])
    _html(folder, results, conditions, graphs, metadata["created"])
    _pdf(folder, results, conditions, graphs, metadata["created"])
    return folder


def _html(folder, results, conditions, graphs, created):
    e = lambda value: escape(str(value))
    blocks = []
    for result, (_, png) in zip(results, graphs):
        rows = "".join(f"<tr><th>{e(k)}</th><td>{e(fmt(v))}</td></tr>" for k, v in result.metrics.items())
        warnings = "".join(f"<li>{e(v)}</li>" for v in result.warnings)
        protocol="".join(f"<tr><th>{e(k)}</th><td>{e(v)}</td></tr>" for k,v in result.run.metadata.items())
        data_rows = "".join("<tr>" + "".join(f"<td>{e(fmt(v))}</td>" for v in
                         (s.idx, s.target, s.reference, s.vf, s.junction, tj, s.seconds)) + "</tr>"
                         for s, tj in zip(result.run.samples, result.junction))
        blocks.append(f"""<section><h2>{e(result.run.path.name)} · {result.run.kind}</h2>
        <p>{e(result.temperature_source)}</p><p class="small">Origem: {e(result.run.path)}<br>SHA-256: {result.run.sha256}<br>
        Linhas ignoradas: {result.run.skipped}</p>
        <img alt="Gráficos do registro" src="data:image/png;base64,{base64.b64encode(png).decode()}">
        <h3>Resultados</h3><table>{rows}</table><h3>Protocolo registrado</h3><table>{protocol}</table><h3>Qualidade e limites</h3><ul>{warnings or '<li>Nenhum aviso adicional.</li>'}</ul>
        <details><summary>Dados utilizados ({len(result.run.samples)} pontos)</summary><div class="scroll"><table>
        <thead><tr><th>Índice</th><th>Alvo °C</th><th>Termopar °C</th><th>Vf V</th><th>Tj gravada °C</th><th>Tj analisada °C</th><th>Tempo s</th></tr></thead>
        <tbody>{data_rows}</tbody></table></div></details></section>""")
    html = f"""<!doctype html><html lang="pt-BR"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
    <title>Caracterização térmica - {e(conditions.dut)}</title><style>
    body{{font:16px/1.6 system-ui,sans-serif;background:#eef3f6;color:#183344;margin:0}}
    main{{max-width:1050px;margin:auto;padding:35px}}header,section{{background:white;padding:30px;border-radius:12px;margin-bottom:22px}}
    h1,h2,h3{{line-height:1.25}}h1{{color:#176b87}}.small{{font-size:12px;color:#516574;overflow-wrap:anywhere}}
    .badge{{background:#ffe5bb;padding:8px 12px;border-radius:6px}}img{{width:100%;height:auto}}
    table{{border-collapse:collapse;width:100%;font-size:13px}}th,td{{padding:9px;border-bottom:1px solid #dae4ea;text-align:left}}
    th{{background:#f0f5f7}}.scroll{{overflow:auto}}summary{{cursor:pointer;color:#176b87}}li{{margin-bottom:5px}}
    @media print{{body{{background:white}}main{{padding:0}}section{{break-before:page}}details{{display:block}}}}
    </style><main><header><p>CONTROL DEVICE · ANÁLISE OFFLINE · v{__version__}</p><h1>Caracterização térmica</h1>
    {'<p class="badge">DEMONSTRAÇÃO - DADOS SINTÉTICOS, SEM VALIDAÇÃO DE HARDWARE</p>' if conditions.synthetic else ''}
    <h2>{e(conditions.dut)}</h2><p>Responsável: {e(conditions.operator or 'Não informado')}<br>Gerado em: {e(created)}</p>
    <p>Ambiente informado: {e(fmt(conditions.ambient))} °C · Potência dissipada informada: {e(fmt(conditions.power))} W<br>
    Regime permanente declarado (TSP: antes do corte; TST: último ponto): {'sim' if conditions.steady else 'não'}</p>
    <p>{e(conditions.notes).replace(chr(10), '<br>')}</p>
    <p class="small">CAL/TST: amostras por alvo. TSP: degrau e curva de resfriamento, com timestamps de software. Não há comprovação automática de estabilidade térmica, incerteza metrológica ou potência real.
    t_rtd_c é o nome legado da coluna do termopar MAX31855. Tempos são os instantes de captura registrados pelo firmware; o arquivo não comprova a janela de 30 µs.
    Não se aplica ganho/offset de termopar novamente aos CSVs, que já contêm a temperatura usada pelo firmware.</p></header>
    {''.join(blocks)}</main></html>"""
    (folder / "relatorio.html").write_text(html, encoding="utf-8")


def _pdf(folder, results, conditions, graphs, created):
    from reportlab.lib import colors
    from reportlab.lib.enums import TA_LEFT
    from reportlab.lib.pagesizes import A4
    from reportlab.lib.styles import getSampleStyleSheet, ParagraphStyle
    from reportlab.lib.units import mm
    from reportlab.pdfbase import pdfmetrics
    from reportlab.pdfbase.ttfonts import TTFont
    from reportlab.platypus import SimpleDocTemplate, Paragraph, Spacer, Table, TableStyle, Image, PageBreak
    import matplotlib
    font_dir = Path(matplotlib.get_data_path()) / "fonts/ttf"
    if "ThermalSans" not in pdfmetrics.getRegisteredFontNames():
        pdfmetrics.registerFont(TTFont("ThermalSans", str(font_dir / "DejaVuSans.ttf")))
        pdfmetrics.registerFont(TTFont("ThermalSansBold", str(font_dir / "DejaVuSans-Bold.ttf")))
        pdfmetrics.registerFontFamily("ThermalSans", normal="ThermalSans", bold="ThermalSansBold")
    styles = getSampleStyleSheet()
    for style in styles.byName.values():
        style.fontName = "ThermalSans"
    styles["Title"].fontName = styles["Heading1"].fontName = styles["Heading2"].fontName = "ThermalSansBold"
    styles["BodyText"].fontSize = 9
    styles["BodyText"].leading = 13
    styles.add(ParagraphStyle(name="SmallData", fontName="ThermalSans", fontSize=7, leading=10, alignment=TA_LEFT))
    P = lambda text, name="BodyText": Paragraph(escape(str(text)).replace("\n", "<br/>"), styles[name])
    story = [P("CARACTERIZAÇÃO TÉRMICA", "Title"), Spacer(1, 8*mm), P(conditions.dut, "Heading1")]
    if conditions.synthetic:
        story += [P("DEMONSTRAÇÃO - DADOS SINTÉTICOS", "Heading2")]
    story += [P("Responsável: " + (conditions.operator or "Não informado")), P("Gerado em: " + created),
              P(f"Ambiente informado: {fmt(conditions.ambient)} °C | Potência dissipada informada: {fmt(conditions.power)} W"),
              P("Regime permanente declarado (TSP: antes do corte; TST: último ponto): " + ("sim" if conditions.steady else "não")),
              Spacer(1, 4*mm), P(conditions.notes), Spacer(1, 8*mm), P("Método e limites", "Heading2"),
              P("Análise de arquivos CAL/TST do firmware STM32. Ajuste da calibração: T = m·Vf + t0, por mínimos quadrados, com temperatura como variável dependente. "
                "A coluna t_rtd_c representa o termopar MAX31855. Os CSVs já contêm a temperatura usada pelo firmware; não se aplica novamente ganho/offset do termopar."),
              P("CAL/TST: registro por alvos. TSP: degrau de potência e leituras de resfriamento; timestamps de software não comprovam acomodação elétrica. Não são inferidos modelos RC, constante de tempo, função de estrutura ou incerteza. "
                "A janela elétrica real precisa ser verificada com instrumentação na bancada."),
              P("Rθj-a requer ambiente independente e regime permanente declarado. Em CAL/TST a potência é informada pelo usuário; em TSP usa-se Ih declarado × Vh medida e o ponto HOT antes do resfriamento. "
                "Estabilidade e condições elétricas reais não são verificadas pelo aplicativo."),
              P("Versão do aplicativo: " + __version__)]
    def table(data, widths, header=False):
        t = Table(data, colWidths=widths, repeatRows=1 if header else 0, hAlign="LEFT")
        commands = [("VALIGN", (0,0),(-1,-1),"TOP"), ("LEFTPADDING",(0,0),(-1,-1),6),
                    ("RIGHTPADDING",(0,0),(-1,-1),6), ("TOPPADDING",(0,0),(-1,-1),5),
                    ("BOTTOMPADDING",(0,0),(-1,-1),5), ("LINEBELOW",(0,0),(-1,-1),.3,colors.HexColor("#d9e3e9"))]
        if header:
            commands.append(("BACKGROUND",(0,0),(-1,0),colors.HexColor("#eaf1f5")))
        t.setStyle(TableStyle(commands))
        return t
    for r, (_, png) in zip(results, graphs):
        story += [PageBreak(), P(r.run.path.name, "Heading1"), P(r.temperature_source), Spacer(1, 3*mm),
                  Image(BytesIO(png), width=174*mm, height=117.6*mm), P("Resultados", "Heading2")]
        metrics = [[P(k, "SmallData"), P(fmt(v), "SmallData")] for k, v in r.metrics.items()]
        if metrics:
            story.append(table(metrics, [132*mm, 42*mm]))
        if r.run.kind=="TSP":
            story += [Spacer(1,4*mm),P("Ensaio TSP e parâmetros registrados","Heading2"),
                P("Neste ensaio Ih=758 mA e Im=2 mA são valores declarados. Vh é medida sob Ih; Tj usa Vf sob Im e a reta própria registrada. MAX31855 mede Tcase. Regime permanente é declaração do operador, não uma consequência do tempo programado. A razão (Tj-Tcase)/P descreve o arranjo medido." )]
            protocol=[[P(k,"SmallData"),P(v,"SmallData")] for k,v in r.run.metadata.items()
                      if not k.startswith(("phase_","cut_us_"))]
            if protocol:story.append(table(protocol,[80*mm,94*mm]))
        story += [PageBreak(), P("Qualidade, limites e dados", "Heading1"),
                  P(r.run.path.name, "Heading2"), P("Qualidade e limites", "Heading2")]
        story.extend(P("• " + warning) for warning in r.warnings)
        story += [Spacer(1, 3*mm), P(f"Origem: {r.run.path}\nSHA-256: {r.run.sha256}\nLinhas ignoradas: {r.run.skipped}", "SmallData"),
                  Spacer(1, 4*mm), P("Dados utilizados", "Heading2")]
        headers = ["Índice", "Alvo °C", "Termopar °C", "Vf V", "Tj grav. °C", "Tj analis. °C", "Tempo s"]
        data = [[P(v, "SmallData") for v in headers]]
        for point, tj in zip(r.run.samples, r.junction):
            data.append([P(fmt(v), "SmallData") for v in (point.idx, point.target, point.reference, point.vf, point.junction, tj, point.seconds)])
        if r.run.kind=="TSP":
            headers=["Índice","Fase","Case °C","Vf Im V","Tj °C","Tempo após SELECT (µs)"]
            data=[[P(v,"SmallData") for v in headers]]
            for point in r.run.samples:
                data.append([P(fmt(v),"SmallData") for v in (point.idx,r.run.metadata.get(f"phase_{point.idx}","?"),point.reference,point.vf,point.junction,r.run.metadata.get(f"cut_us_{point.idx}","?"))])
            story.append(table(data,[12*mm,29*mm,27*mm,28*mm,28*mm,50*mm],True))
        else:
            story.append(table(data, [15*mm, 24*mm, 29*mm, 23*mm, 28*mm, 30*mm, 25*mm], True))
    def footer(canvas, doc):
        canvas.saveState()
        canvas.setFont("ThermalSans", 8)
        canvas.setFillColor(colors.HexColor("#53697a"))
        canvas.drawString(18*mm, 12*mm, "Control Device | Análise offline" + (" | DADOS SINTÉTICOS" if conditions.synthetic else ""))
        canvas.drawRightString(192*mm, 12*mm, str(doc.page))
        canvas.restoreState()
    doc = SimpleDocTemplate(str(folder / "relatorio.pdf"), pagesize=A4,
                            leftMargin=18*mm, rightMargin=18*mm, topMargin=17*mm, bottomMargin=20*mm,
                            title="Caracterização térmica", author=conditions.operator)
    doc.build(story, onFirstPage=footer, onLaterPages=footer)
