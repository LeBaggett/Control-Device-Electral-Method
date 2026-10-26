"""Cálculos explícitos, usando a mesma orientação T = m*Vf + t0 do firmware."""
from __future__ import annotations
from dataclasses import dataclass, field
import math
from .data import Calibration, Run


@dataclass
class Conditions:
    dut: str = "Não informado"
    operator: str = ""
    notes: str = ""
    ambient: float | None = None
    power: float | None = None
    steady: bool = False
    synthetic: bool = False

    def validate(self):
        for label, value in (("Temperatura ambiente", self.ambient), ("Potência", self.power)):
            if value is not None and not math.isfinite(value):
                raise ValueError(label + " deve ser finita.")
        if self.power is not None and self.power <= 0:
            raise ValueError("Potência deve ser maior que zero.")


@dataclass
class Analysis:
    run: Run
    calibration: Calibration | None
    junction: list[float | None]
    temperature_source: str
    metrics: dict[str, float] = field(default_factory=dict)
    warnings: list[str] = field(default_factory=list)


def fit_calibration(run: Run) -> Calibration:
    if run.kind != "CAL" or len(run.samples) < 2:
        raise ValueError("Selecione uma calibração com pelo menos dois pontos válidos.")
    x = [r.vf for r in run.samples]
    y = [r.reference for r in run.samples]
    mx, my = math.fsum(x)/len(x), math.fsum(y)/len(y)
    xx = math.fsum((v-mx)**2 for v in x)
    yy = math.fsum((v-my)**2 for v in y)
    if xx <= 1e-20 or yy <= 1e-20:
        raise ValueError("A calibração precisa de variação de Vf e de temperatura.")
    xy = math.fsum((a-mx)*(b-my) for a, b in zip(x, y))
    slope = xy/xx
    if abs(slope) < 1e-6:
        raise ValueError("Sensibilidade insuficiente para inferir temperatura.")
    return Calibration(run.path.name + " [ajuste no PC]", slope, my-slope*mx,
                       min(1., max(0., xy*xy/(xx*yy))), len(x), min(x), max(x), run.sha256)


def analyze(run: Run, selected: Calibration | None = None, conditions: Conditions | None = None) -> Analysis:
    conditions = conditions or Conditions()
    conditions.validate()
    if run.kind=="TSP":
        return analyze_sofia(run,conditions,selected)
    warnings = list(run.warnings)
    cal = None
    if run.kind == "CAL":
        try:
            cal = fit_calibration(run)
        except ValueError as exc:
            warnings.append(str(exc))
        junction = [cal.temperature(r.vf) if cal else None for r in run.samples]
        source = "Ajuste da própria calibração; resíduos não são validação independente."
    else:
        cal = selected
        junction = ([cal.temperature(r.vf) for r in run.samples] if cal else [r.junction for r in run.samples])
        source = "Recalculada explicitamente com " + cal.name if cal else "Tj gravada pelo firmware (tj_c)."
        if cal:
            warnings.append("A calibração foi associada manualmente no PC; o CSV não contém um identificador que comprove esse vínculo.")
            if cal.vf_min is not None and any(not cal.vf_min <= r.vf <= cal.vf_max for r in run.samples):
                warnings.append("Há Vf fora da faixa de calibração: Tj contém extrapolação.")
    out = Analysis(run, cal, junction, source, warnings=warnings)
    if not run.samples:
        return out
    out.metrics["Pontos válidos"] = len(run.samples)
    out.metrics["Termopar mínimo (°C)"] = min(r.reference for r in run.samples)
    out.metrics["Termopar máximo (°C)"] = max(r.reference for r in run.samples)
    errors = [r.reference-r.target for r in run.samples]
    out.metrics["Maior desvio termopar-alvo (°C)"] = max(abs(e) for e in errors)
    tj = [v for v in junction if v is not None]
    if tj:
        out.metrics["Tj mínima (°C)"] = min(tj)
        out.metrics["Tj máxima (°C)"] = max(tj)
    residuals = [v-r.reference for r, v in zip(run.samples, junction) if v is not None]
    if residuals:
        label = "Resíduo do ajuste" if run.kind == "CAL" else "Diferença Tj-termopar"
        out.metrics[label + " RMSE (°C)"] = math.sqrt(math.fsum(v*v for v in residuals)/len(residuals))
        out.metrics[label + " média (°C)"] = math.fsum(residuals)/len(residuals)
        out.metrics[label + " máximo absoluto (°C)"] = max(abs(v) for v in residuals)
    if cal:
        out.metrics.update({"m (°C/V)": cal.slope, "t0 (°C)": cal.intercept, "R²": cal.r2,
                            "dVf/dT (mV/°C)": 1000/cal.slope})
        if cal.n <= 2:
            out.warnings.append("Ajuste com dois pontos: R² pode ser 1 sem demonstrar linearidade em toda a faixa.")
        if cal.r2 < .98:
            out.warnings.append("R² abaixo de 0,98; conferir linearidade, ruído e condições de medição.")
        if cal.slope >= 0:
            out.warnings.append("Sensibilidade positiva de Vf com temperatura; verificar se é esperada para o DUT e a corrente usada.")
    if run.kind == "TST":
        out.warnings.append("Tj e termopar podem representar locais e instantes diferentes; sua diferença não é, por si só, erro de calibração.")
        out.warnings.append("Constantes de tempo e Zth não calculados: faltam curva contínua, potência ao longo do tempo e protocolo de degrau térmico.")
        times = [r.seconds for r in run.samples if r.seconds is not None]
        if times:
            out.metrics["Último instante registrado (s)"] = times[-1]
        if conditions.steady and conditions.power is not None and conditions.ambient is not None and junction and junction[-1] is not None:
            delta = junction[-1]-conditions.ambient
            if delta < 0:
                out.warnings.append("Tj final inferior à temperatura ambiente; resistência térmica não calculada.")
            else:
                out.metrics["Rθj-a estimada no ponto final (°C/W)"] = delta/conditions.power
                out.warnings.append("Rθj-a usa potência dissipada e ambiente informados pelo usuário, com regime permanente declarado; não são medições presentes no CSV.")
        else:
            out.warnings.append("Rθj-a não calculada: exige Tj final válida, potência dissipada, temperatura ambiente e confirmação de regime permanente.")
    return out


def analyze_sofia(run, conditions, selected=None):
    """Independent step/cooling protocol; never substitute an unrelated CAL."""
    out=Analysis(run,None,[p.junction for p in run.samples],
                 "Ensaio TSP: Tj sob 2 mA, reta T=m*Vf+b registrada no CSV; MAX31855 na caixa.",
                 warnings=list(run.warnings))
    out.warnings += [
        "Correntes fixas do dispositivo: aquecimento 758 mA e medição 2 mA, conforme informado pelo responsável. Não há ajuste por software nem leitura de corrente.",
        "Duração de aquecimento não comprova regime permanente. Confirmar estabilidade na bancada.",
        "Tempos de comutação são marcadores de software DWT; não comprovam acomodação elétrica nem tempo físico do MOSFET.",
        "Leituras de resfriamento têm instantes reais registrados; intervalos solicitados podem ser perdidos pelo superloop.",
        "A razão (Tj-Tcase)/P descreve o arranjo medido; TB não é medido.",
        "Não há extrapolação para t=0, ajuste RC ou função de estrutura.",
    ]
    if selected is not None:out.warnings.append("Calibração selecionada no PC ignorada para preservar a reta do ensaio TSP.")
    def value(key):
        try:
            n=float(run.metadata[key])
            return n if math.isfinite(n) else None
        except (ValueError,KeyError):return None
    names={"current_heating_a":"Corrente fixa de aquecimento (A)",
           "current_measurement_a":"Corrente fixa de medição (A)",
           "slope_c_per_v":"m assinado (°C/V)","intercept_c":"b (°C)",
           "heat_requested_s":"Aquecimento solicitado (s)","heat_actual_s":"Aquecimento observado (s)",
           "delay_requested_us":"Espera solicitada após SELECT (µs)",
           "delay_first_us":"Início conversão após SELECT, software (µs)",
           "delay_end_us":"Fim conversão após SELECT, software (µs)",
           "heating_voltage_v":"Vf sob corrente de aquecimento (V)"}
    for key,label in names.items():
        v=value(key)
        if v is not None:out.metrics[label]=v
    out.metrics["Pontos registrados"]=len(run.samples)
    hot=next((p for p in run.samples if run.metadata.get(f"phase_{p.idx}")=="HOT"),None)
    baseline=next((p for p in run.samples if run.metadata.get(f"phase_{p.idx}")=="BASELINE"),None)
    ih,vh=value("current_heating_a"),value("heating_voltage_v")
    im=value("current_measurement_a")
    fixed_ok=ih is not None and im is not None and math.isclose(ih,.758,abs_tol=1e-6) and math.isclose(im,.002,abs_tol=1e-6)
    if not fixed_ok:out.warnings.append("Correntes do arquivo ausentes ou diferentes das correntes fixas 758/2 mA: potência e razões térmicas recusadas.")
    # Vh foi medida durante aquecimento; Vf sob 2 mA serve para termometria.
    # A potencia do formulario nao substitui esta potencia do protocolo TSP.
    power=.758*vh if fixed_ok and vh and vh>0 else None
    if power is not None:
        out.metrics["Potência aquecimento Ih declarado × Vh medida (W)"]=power
        stored=value("heating_power_w")
        if stored is not None and abs(stored-power)>.00001:out.warnings.append("Potência gravada diverge de Ih × Vh.")
    m,b=value("slope_c_per_v"),value("intercept_c")
    if m is not None and b is not None:
        for point in run.samples:
            if point.junction is not None and abs(point.junction-(m*point.vf+b))>.05:
                out.warnings.append("Tj gravada diverge da reta registrada; conferir arquivo.");break
    if not hot or hot.junction is None:
        out.warnings.append("Ensaio incompleto: sem ponto HOT válido; não há resultado térmico final.")
        return out
    out.metrics.update({"Vf quente sob Im (V)":hot.vf,"Tj quente medida após corte (°C)":hot.junction,
                        "Temperatura da caixa no corte (°C)":hot.reference})
    if baseline and baseline.junction is not None:
        out.metrics["Tj inicial sob Im (°C)"]=baseline.junction
        out.metrics["Elevação de Tj observada (°C)"]=hot.junction-baseline.junction
    # Usar o ponto HOT, nao a ultima amostra ja resfriada. A razao com a caixa
    # descreve o arranjo medido e exige a declaracao de regime pelo operador.
    if power and conditions.steady:
        out.metrics["Razão aparente (Tj-Tcase)/P do arranjo (°C/W)"]=(hot.junction-hot.reference)/power
        if conditions.ambient is not None and hot.junction>=conditions.ambient:
            out.metrics["Rθj-a estimada, regime declarado (°C/W)"]=(hot.junction-conditions.ambient)/power
        else:out.warnings.append("Rθj-a indisponível: informe ambiente independente e coerente; o MAX31855 mede a caixa.")
    else:out.warnings.append("Razões térmicas estáticas não calculadas: regime permanente não declarado ou potência indisponível.")
    if conditions.power is not None:out.warnings.append("Potência informada no formulário não substitui Ih × Vh do ensaio TSP.")
    return out
