"""Prepara caches locais; não instala nem altera o Python global."""
from pathlib import Path
import os
import sys
import tempfile
import zipfile


def prepare_environment():
    cache = Path(tempfile.gettempdir()) / "ControlDeviceThermalAnalyzer"
    cache.mkdir(exist_ok=True)
    mpl_cache = cache / "matplotlib"
    mpl_cache.mkdir(exist_ok=True)
    os.environ.setdefault("MPLCONFIGDIR", str(mpl_cache))
    # Reaproveita scripts compactados da própria instalação Tcl/Tk, quando
    # a biblioteca nativa não resolve automaticamente esse layout. Sem downloads.
    import _tkinter
    base = Path(sys.base_prefix) / "tcl"
    for prefix, library, env, version in (("libtcl", "tcl_library", "TCL_LIBRARY", _tkinter.TCL_VERSION),
                                          ("libtk", "tk_library", "TK_LIBRARY", _tkinter.TK_VERSION)):
        if os.environ.get(env):
            continue
        standard = base / (("tcl" if prefix == "libtcl" else "tk") + version)
        if (standard / ("init.tcl" if prefix == "libtcl" else "tk.tcl")).exists():
            continue
        archives = sorted(base.glob(prefix + version + "*.zip"))
        if not archives:
            continue
        archive = archives[-1]
        destination = cache / archive.stem
        marker = destination / library / ("init.tcl" if prefix == "libtcl" else "tk.tcl")
        if not marker.exists():
            destination.mkdir(exist_ok=True)
            with zipfile.ZipFile(archive) as source:
                for item in source.infolist():
                    target = (destination / item.filename).resolve()
                    if not target.is_relative_to(destination.resolve()):
                        raise ValueError("Caminho inválido no pacote Tcl/Tk instalado.")
                source.extractall(destination)
        os.environ[env] = str(destination / library)
