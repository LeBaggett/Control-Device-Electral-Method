from __future__ import annotations
import math
from matplotlib.figure import Figure
from .analysis import Analysis

BLUE, ORANGE, GREEN = "#176b87", "#d97a21", "#26836b"


def make_figure(result: Analysis) -> Figure:
    if result.run.kind=="TSP":
        return sofia_figure(result)
    fig = Figure(figsize=(10.8, 7.3), dpi=110, layout="constrained", facecolor="white")
    axes = fig.subplots(2, 2)
    rows = result.run.samples
    fig.suptitle(result.run.path.name + " | " + ("Calibração" if result.run.kind == "CAL" else "Ensaio por alvos"), fontsize=14, fontweight="bold")
    if not rows:
        for ax in axes.flat:
            ax.set_axis_off()
        axes[0, 0].text(.05, .5, "Sem amostras válidas. Consulte os avisos.", transform=axes[0, 0].transAxes)
        return fig
    vf, tc = [r.vf for r in rows], [r.reference for r in rows]
    tj = [v if v is not None else math.nan for v in result.junction]
    ax = axes[0, 0]
    ax.scatter(vf, tc, s=24, c=BLUE, label="Termopar")
    if result.calibration:
        cal = result.calibration
        limits = [min(vf), max(vf)]
        ax.plot(limits, [cal.temperature(v) for v in limits], color=ORANGE, label="T = m·Vf + t0")
    ax.set(xlabel="Vf (V)", ylabel="Temperatura (°C)", title="Relação temperatura × Vf")
    ax.legend(fontsize=8)
    timed = result.run.kind == "TST" and all(r.seconds is not None for r in rows)
    x = [r.seconds if timed else r.idx for r in rows]
    x_label = "Tempo desde o início (s)" if timed else "Índice registrado"
    ax = axes[0, 1]
    ax.plot(x, tc, "o-", color=BLUE, label="Termopar", markersize=4)
    ax.plot(x, [r.target for r in rows], "--", color="#8b96a2", label="Alvo")
    if any(v is not None for v in result.junction):
        ax.plot(x, tj, "s-", color=ORANGE, label="T ajustada" if result.run.kind == "CAL" else "Tj", markersize=3)
    ax.set(xlabel=x_label, ylabel="Temperatura (°C)", title="Pontos registrados\n(guias entre amostras)")
    ax.legend(fontsize=8)
    ax = axes[1, 0]
    ax.plot(x, vf, "o-", color=GREEN, markersize=4)
    ax.set(xlabel=x_label, ylabel="Vf (V)", title="Tensão direta registrada")
    ax = axes[1, 1]
    ax.axhline(0, color="#8b96a2", lw=1)
    ax.plot(tc, [v-t for v, t in zip(tj, tc)], "o", color=ORANGE)
    ax.set(xlabel="Termopar (°C)", ylabel="Diferença (°C)",
           title="Resíduos do ajuste" if result.run.kind == "CAL" else "Tj − termopar\n(não é erro metrológico)")
    for ax in axes.flat:
        ax.grid(True, alpha=.18)
        ax.tick_params(labelsize=8)
        ax.title.set_fontsize(10)
        for side in ("top", "right"):
            ax.spines[side].set_visible(False)
    return fig


def sofia_figure(result):
    fig=Figure(figsize=(10.8,7.3),dpi=110,layout="constrained",facecolor="white")
    axes=fig.subplots(2,2)
    fig.suptitle(result.run.path.name + " | Degrau de potencia TSP / resfriamento",fontsize=14,fontweight="bold")
    rows=[]
    for point in result.run.samples:
        if result.run.metadata.get(f"phase_{point.idx}") not in ("HOT","COOLING"):continue
        try:elapsed=float(result.run.metadata[f"cut_us_{point.idx}"])*1e-6
        except (KeyError,ValueError):continue
        if elapsed>0 and math.isfinite(elapsed):rows.append((elapsed,point))
    x=[t for t,p in rows]
    tj=[p.junction if p.junction is not None else math.nan for t,p in rows]
    axes[0,0].plot(x,tj,"o-",color=ORANGE,markersize=4,label="Tj sob Im")
    axes[0,0].plot(x,[p.reference for t,p in rows],"s--",color=BLUE,markersize=3,label="Case MAX31855")
    axes[0,0].set(title="Resfriamento após o corte",ylabel="Temperatura (°C)");axes[0,0].legend(fontsize=8)
    axes[0,1].plot(x,[p.vf for t,p in rows],"o-",color=GREEN,markersize=4)
    axes[0,1].set(title="Tensão sob corrente de medição",ylabel="Vf (V)")
    axes[1,0].plot(x,[v-p.reference for v,(t,p) in zip(tj,rows)],"o-",color=ORANGE,markersize=4)
    axes[1,0].set(title="Diferença observada",ylabel="Tj − Tcase (°C)")
    for ax in (axes[0,0],axes[0,1],axes[1,0]):
        ax.set(xscale="log",xlabel="Tempo observado desde SELECT (s)")
        ax.grid(True,which="both",alpha=.18);ax.tick_params(labelsize=8);ax.title.set_fontsize(10)
    axes[1,1].set_axis_off()
    keys=["current_heating_a","current_measurement_a","slope_c_per_v","intercept_c","heating_voltage_v","heating_power_w","heat_actual_s","delay_requested_us","delay_first_us","delay_end_us"]
    lines=["Parâmetros registrados"]+[k+": "+result.run.metadata.get(k,"ausente") for k in keys]
    axes[1,1].text(0,.98,"\n".join(lines),va="top",fontsize=9,transform=axes[1,1].transAxes)
    return fig
