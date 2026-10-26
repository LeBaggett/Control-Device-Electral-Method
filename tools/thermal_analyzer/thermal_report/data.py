"""Importação sem escrita no SD; mantém origem, erros de linha e hashes."""
from __future__ import annotations
import csv
import hashlib
import io
import math
import zlib
from dataclasses import dataclass, field
from pathlib import Path

# t_rtd_c e o nome historico da coluna; no dispositivo atual ela representa
# a temperatura da caixa lida pelo termopar. Preservar para ler CSVs existentes.
CAL_COLUMNS = ("idx", "t_alvo_c", "t_rtd_c", "vf_v")
TEST_COLUMNS = CAL_COLUMNS + ("tj_c", "t_s")


@dataclass(frozen=True)
class Sample:
    idx: int
    target: float
    reference: float
    vf: float
    junction: float | None = None
    seconds: float | None = None


@dataclass
class Run:
    path: Path
    kind: str
    samples: list[Sample]
    sha256: str
    warnings: list[str] = field(default_factory=list)
    skipped: int = 0
    metadata: dict[str, str] = field(default_factory=dict)


@dataclass(frozen=True)
class Calibration:
    name: str
    slope: float
    intercept: float
    r2: float
    n: int
    vf_min: float | None = None
    vf_max: float | None = None
    source_sha256: str = ""
    tc_gain: float | None = None
    tc_offset: float | None = None

    def temperature(self, vf: float) -> float:
        return self.slope * vf + self.intercept


def number(text: str, delimiter: str = ",", optional: bool = False) -> float | None:
    value = text.strip()
    if delimiter == ";":
        value = value.replace(",", ".")
    if optional and value.lower() in {"", "nan", "na", "n/a"}:
        return None
    result = float(value)
    if not math.isfinite(result):
        if optional:
            return None
        raise ValueError("valor não finito")
    if abs(result) > 1e12:
        raise ValueError("valor fora do limite numérico de importação")
    return result


def read_run(path: str | Path) -> Run:
    path = Path(path)
    raw = path.read_bytes()
    warnings = []
    try:
        text = raw.decode("utf-8-sig")
    except UnicodeDecodeError:
        text = raw.decode("cp1252")
        warnings.append("Arquivo decodificado como Windows-1252.")
    lines = [(i, line) for i, line in enumerate(text.splitlines(), 1)
             if line.strip() and not line.lstrip().startswith("#")]
    if not lines:
        raise ValueError("arquivo vazio")
    delimiter = ";" if ";" in lines[0][1] else ","
    header = tuple(x.strip().lower() for x in next(csv.reader([lines[0][1]], delimiter=delimiter)))
    if header not in (CAL_COLUMNS, TEST_COLUMNS):
        raise ValueError("cabeçalho não reconhecido; esperado CAL ou TST do firmware")
    result = Run(path.resolve(), "CAL" if header == CAL_COLUMNS else "TST", [],
                 hashlib.sha256(raw).hexdigest(), warnings)
    for comment in text.splitlines():
        if comment.lstrip().startswith("#"):
            for item in comment.lstrip()[1:].split(","):
                key, sep, value = item.partition("=")
                if sep:
                    key=key.strip()
                    if key in result.metadata:
                        result.warnings.append("Metadado repetido: " + key)
                    result.metadata[key]=value.strip()
    if result.metadata.get("mode") in ("TSP_STEP","SOFIA_STEP"):
        result.kind="TSP"
    for line_no, line in lines[1:]:
        try:
            fields = next(csv.reader([line], delimiter=delimiter))
            if len(fields) != len(header):
                raise ValueError("quantidade de colunas diferente do cabeçalho")
            index = number(fields[0], delimiter)
            if index < 0 or not index.is_integer():
                raise ValueError("índice inválido")
            values = [number(x, delimiter) for x in fields[1:4]]
            optional = ([number(x, delimiter, True) for x in fields[4:]]
                        if header == TEST_COLUMNS else [None, None])
            result.samples.append(Sample(int(index), *values, *optional))
        except (ValueError, csv.Error) as exc:
            result.skipped += 1
            result.warnings.append(f"Linha {line_no} ignorada: {exc}.")
    if not result.samples:
        result.warnings.append("Arquivo sem amostras válidas; pode ser um ensaio interrompido.")
    rows = result.samples
    if any(b.idx <= a.idx for a, b in zip(rows, rows[1:])):
        result.warnings.append("Índices repetidos ou fora de ordem; ordem original preservada.")
    if any(b.idx != a.idx + 1 for a, b in zip(rows, rows[1:])):
        result.warnings.append("Há lacunas ou repetições na sequência de índices.")
    timed = [r.seconds for r in rows if r.seconds is not None]
    if any(t < 0 for t in timed) or any(b <= a for a, b in zip(timed, timed[1:])):
        result.warnings.append("Tempos negativos, repetidos ou regressivos; não interpretar taxa temporal.")
    if any(r.vf <= 0 or r.vf > 3.6 for r in rows):
        result.warnings.append("Vf fora da faixa usual de 0 a 3,6 V; conferir ligação e escala do ADC.")
    if result.kind == "TST":
        result.warnings.append("Registro por alvos: as linhas entre pontos são apenas guias visuais, não medições contínuas.")
        if any(r.junction is None for r in rows):
            result.warnings.append("Há Tj ausente/NaN no registro; não é substituída por zero.")
        if any(r.seconds is None for r in rows):
            result.warnings.append("Há tempo ausente; o gráfico usa o índice, sem inventar instantes de aquisição.")
    return result


def read_calibration(path: str | Path) -> Calibration:
    """Lê CALIB.TXT/BAK e valida o CRC sobre os bytes originais, inclusive CRLF."""
    path = Path(path)
    raw = path.read_bytes()
    marker = raw.rfind(b"#crc=")
    if marker < 0:
        raise ValueError("CALIB sem CRC; não será aplicado")
    suffix = raw[marker:].decode("ascii").strip()
    stored = int(suffix.removeprefix("#crc="), 16)
    if stored != zlib.crc32(raw[:marker]) & 0xFFFFFFFF:
        raise ValueError("CRC de CALIB inválido; não será aplicado")
    lines = raw[:marker].decode("ascii").splitlines()
    if len(lines) < 2 or lines[0] != "m,t0,r2,n":
        raise ValueError("formato de CALIB não reconhecido")
    m, t0, r2, count = lines[1].split(",")
    slope, intercept, r_squared = float(m), float(t0), float(r2)
    n = int(count)
    if not all(math.isfinite(x) for x in (slope, intercept, r_squared)) or abs(slope) < 1e-6 or not 0 <= r_squared <= 1 or n < 2:
        raise ValueError("coeficientes inválidos em CALIB")
    tc = [None, None]
    for line in lines[2:]:
        if line.startswith("#tc="):
            tc = [float(x) for x in line[4:].split(",")]
            if len(tc) != 2 or not all(math.isfinite(x) for x in tc):
                raise ValueError("metadados de termopar inválidos")
    return Calibration(path.name + " [CRC válido]", slope, intercept, r_squared, n,
                       source_sha256=hashlib.sha256(raw).hexdigest(), tc_gain=tc[0], tc_offset=tc[1])
