"""Tkinter: controles da interface separados dos cálculos e exportação."""
from __future__ import annotations
from datetime import datetime
from pathlib import Path
import threading
import queue
import tkinter as tk
from tkinter import ttk, filedialog, messagebox
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2Tk
from .data import read_run, read_calibration
from .analysis import analyze, fit_calibration, Conditions
from .plots import make_figure
from .report import export_bundle, fmt


class Application(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Caracterização térmica | Control Device")
        self.geometry("1320x880")
        self.minsize(1050, 720)
        self.runs, self.models = [], {}
        self.canvas = self.toolbar = None
        self.exporting = False
        self.messages = queue.Queue()
        self.status = tk.StringVar(value="Importe uma pasta do SD ou abra os dados de demonstração.")
        self.dut = tk.StringVar()
        self.operator = tk.StringVar()
        self.ambient = tk.StringVar()
        self.power = tk.StringVar()
        self.steady = tk.BooleanVar()
        self.synthetic = tk.BooleanVar()
        self.calibration = tk.StringVar(value="Usar tj_c gravada no SD")
        style = ttk.Style(self)
        style.theme_use("clam")
        style.configure("Treeview", rowheight=27)
        style.configure("TButton", padding=7)
        style.configure("Title.TLabel", font=("Segoe UI", 18, "bold"), foreground="#176b87")
        top = ttk.Frame(self, padding=12)
        top.pack(fill="x")
        ttk.Label(top, text="Caracterização térmica", style="Title.TLabel").pack(anchor="w", pady=(0, 8))
        actions = ttk.Frame(top)
        actions.pack(fill="x")
        for label, action in (("Importar pasta do SD", self.open_folder), ("Adicionar CSVs", self.open_files),
                              ("Demonstração", self.demo), ("Limpar", self.clear), ("Exportar relatório", self.export)):
            ttk.Button(actions, text=label, command=action).pack(side="left", padx=4)
        body = ttk.Panedwindow(self, orient="horizontal")
        body.pack(fill="both", expand=True, padx=12)
        left = ttk.Frame(body, padding=(0, 0, 12, 0))
        right = ttk.Frame(body)
        body.add(left, weight=1)
        body.add(right, weight=4)
        left_tabs = ttk.Notebook(left)
        left_tabs.pack(fill="both", expand=True)
        files_panel, conditions_panel = ttk.Frame(left_tabs, padding=6), ttk.Frame(left_tabs, padding=6)
        left_tabs.add(files_panel, text="Arquivos")
        left_tabs.add(conditions_panel, text="Condições")
        ttk.Label(files_panel, text="Ctrl/clique: selecionar vários").pack(anchor="w", pady=5)
        self.files = ttk.Treeview(files_panel, columns=("type", "n"), show="tree headings", height=9, selectmode="extended")
        self.files.heading("#0", text="Arquivo")
        self.files.heading("type", text="Tipo")
        self.files.heading("n", text="Pontos")
        self.files.column("#0", width=175)
        self.files.column("type", width=45, stretch=False)
        self.files.column("n", width=50, stretch=False)
        self.files.pack(fill="both", expand=True)
        self.files.bind("<<TreeviewSelect>>", lambda _: self.refresh())
        ttk.Label(files_panel, text="Calibração para recalcular Tj").pack(anchor="w", pady=(12, 3))
        self.cal_combo = ttk.Combobox(files_panel, state="readonly", textvariable=self.calibration)
        self.cal_combo.pack(fill="x")
        self.cal_combo.bind("<<ComboboxSelected>>", lambda _: self.refresh())
        ttk.Button(files_panel, text="Carregar CALIB.TXT/BAK (CRC)", command=self.open_model).pack(fill="x", pady=4)
        fields = ttk.LabelFrame(conditions_panel, text="Condições informadas", padding=9)
        fields.pack(fill="x", pady=8)
        for title, var in (("Identificação do DUT", self.dut), ("Responsável", self.operator),
                           ("Ambiente (°C) — opcional", self.ambient), ("Potência dissipada (W) — opcional", self.power)):
            ttk.Label(fields, text=title).pack(anchor="w", pady=(5, 1))
            ttk.Entry(fields, textvariable=var).pack(fill="x")
        ttk.Checkbutton(fields, text="Confirmo regime permanente\nTSP: antes do corte\nTST: no último ponto", variable=self.steady, command=self.refresh).pack(anchor="w", pady=8)
        ttk.Checkbutton(fields, text="Marcar como dados sintéticos", variable=self.synthetic).pack(anchor="w")
        ttk.Label(fields, text="Observações / corrente de medição / montagem", wraplength=280).pack(anchor="w", pady=(8, 3))
        self.notes = tk.Text(fields, height=4, width=28, wrap="word", font=("Segoe UI", 9))
        self.notes.pack(fill="x")
        ttk.Button(fields, text="Atualizar análise", command=self.refresh).pack(fill="x", pady=(8, 0))
        notebook = ttk.Notebook(right)
        notebook.pack(fill="both", expand=True)
        self.plot_frame = ttk.Frame(notebook)
        self.summary = tk.Text(notebook, wrap="word", font=("Consolas", 10), padx=16, pady=14)
        data_frame = ttk.Frame(notebook)
        notebook.add(self.plot_frame, text="Gráficos")
        notebook.add(self.summary, text="Resultados e avisos")
        notebook.add(data_frame, text="Dados")
        names = ("idx", "Alvo °C", "Termopar °C", "Vf V", "Tj gravada °C", "Tj analisada °C", "Tempo s")
        self.table = ttk.Treeview(data_frame, columns=names, show="headings")
        for col in names:
            self.table.heading(col, text=col)
            self.table.column(col, width=110, anchor="e")
        scrollbar = ttk.Scrollbar(data_frame, orient="vertical", command=self.table.yview)
        self.table.configure(yscrollcommand=scrollbar.set)
        scrollbar.pack(side="right", fill="y")
        self.table.pack(fill="both", expand=True)
        ttk.Label(self, textvariable=self.status, padding=10, wraplength=1250).pack(fill="x")
        self.rebuild_models()
        self.after(150, self.poll_export)

    def conditions(self):
        def optional(var):
            text = var.get().strip().replace(",", ".")
            return float(text) if text else None
        c = Conditions(self.dut.get().strip() or "DUT não informado", self.operator.get().strip(),
                       self.notes.get("1.0", "end-1c"), optional(self.ambient), optional(self.power),
                       self.steady.get(), self.synthetic.get())
        c.validate()
        return c

    def rebuild_models(self):
        for run in self.runs:
            if run.kind == "CAL":
                try:
                    model = fit_calibration(run)
                    self.models[str(run.path)] = model
                except ValueError:
                    pass
        values = ["Usar tj_c gravada no SD"] + [f"{i+1}. {m.name}" for i, m in enumerate(self.models.values())]
        self.cal_combo["values"] = values
        if self.calibration.get() not in values:
            self.calibration.set(values[0])

    def selected_model(self):
        values = list(self.cal_combo["values"])
        index = values.index(self.calibration.get())
        return list(self.models.values())[index-1] if index else None

    def import_paths(self, paths):
        if self.exporting:
            return
        errors = []
        existing = {r.path for r in self.runs}
        for path in paths:
            try:
                if Path(path).resolve() in existing:
                    continue
                run = read_run(path)
                self.runs.append(run)
                existing.add(run.path)
                self.files.insert("", "end", iid=str(len(self.runs)-1), text=run.path.name,
                                  values=(run.kind, len(run.samples)))
            except (ValueError, OSError) as exc:
                errors.append(f"{Path(path).name}: {exc}")
        self.rebuild_models()
        if self.runs:
            self.files.selection_set(str(len(self.runs)-1))
        self.status.set(f"{len(self.runs)} registros carregados. Os arquivos originais são somente leitura para este aplicativo.")
        if errors:
            messagebox.showwarning("Arquivos não importados", "\n".join(errors[:20]))

    def open_folder(self):
        folder = filedialog.askdirectory(title="Pasta do cartão SD ou cópia local")
        if folder:
            paths = sorted(p for p in Path(folder).iterdir() if p.is_file() and p.suffix.lower() == ".csv")
            if not paths:
                messagebox.showinfo("Importação", "Nenhum CSV encontrado nesta pasta.")
            self.import_paths(paths)

    def open_files(self):
        self.import_paths(filedialog.askopenfilenames(filetypes=[("CSV", "*.csv *.CSV"), ("Todos", "*.*")]))

    def open_model(self):
        filename = filedialog.askopenfilename(filetypes=[("Calibração", "*.TXT *.txt *.BAK *.bak"), ("Todos", "*.*")])
        if filename:
            try:
                self.models[str(Path(filename).resolve())] = read_calibration(filename)
                self.rebuild_models()
                self.status.set("Calibração com CRC válido importada. Selecione-a explicitamente para aplicar aos ensaios.")
            except (ValueError, OSError) as exc:
                messagebox.showerror("Calibração não aplicada", str(exc))

    def clear(self):
        if self.exporting:
            return
        self.runs.clear()
        self.models.clear()
        self.files.delete(*self.files.get_children())
        self.table.delete(*self.table.get_children())
        self.summary.delete("1.0", "end")
        self.discard_plot()
        self.rebuild_models()
        self.status.set("Lista limpa; os arquivos de origem não foram alterados.")

    def demo(self):
        if self.exporting:
            return
        self.clear()
        self.synthetic.set(True)
        self.dut.set("Diodo de demonstração")
        self.import_paths(sorted((Path(__file__).resolve().parents[1]/"examples").glob("*.CSV")))
        self.status.set("DEMONSTRAÇÃO: dados sintéticos, não representam medições de hardware.")

    def refresh(self):
        if self.exporting:
            return
        selected = self.files.selection()
        if not selected:
            return
        try:
            result = analyze(self.runs[int(selected[0])], self.selected_model(), self.conditions())
        except ValueError as exc:
            self.status.set("Confira as condições: " + str(exc))
            return
        self.discard_plot()
        self.canvas = FigureCanvasTkAgg(make_figure(result), master=self.plot_frame)
        self.toolbar = NavigationToolbar2Tk(self.canvas, self.plot_frame)
        self.toolbar.update()
        self.canvas.get_tk_widget().pack(fill="both", expand=True)
        self.canvas.draw_idle()
        self.summary.delete("1.0", "end")
        text = result.run.path.name + "\n" + result.temperature_source + "\n\n"
        text += "\n".join(f"{k}: {fmt(v)}" for k, v in result.metrics.items())
        text += "\n\nAVISOS\n" + "\n".join("• " + w for w in result.warnings)
        text += f"\n\nSHA-256: {result.run.sha256}\nOrigem: {result.run.path}"
        self.summary.insert("end", text)
        self.table.delete(*self.table.get_children())
        for row, tj in zip(result.run.samples, result.junction):
            self.table.insert("", "end", values=[fmt(x) for x in (row.idx, row.target, row.reference, row.vf, row.junction, tj, row.seconds)])

    def discard_plot(self):
        if self.canvas:
            self.canvas.toolbar = None
            self.canvas.figure.clear()
            self.canvas = None
        for child in self.plot_frame.winfo_children():
            child.destroy()
        self.toolbar = None

    def export(self):
        if self.exporting:
            return
        chosen = self.files.selection()
        if not chosen:
            messagebox.showinfo("Relatório", "Selecione um ou mais registros na lista.")
            return
        try:
            conditions = self.conditions()
            results = [analyze(self.runs[int(i)], self.selected_model(), conditions) for i in chosen]
        except ValueError as exc:
            messagebox.showerror("Confira as condições", str(exc))
            return
        directory = filedialog.askdirectory(title="Onde salvar o pacote do relatório?")
        if not directory:
            return
        folder = Path(directory)/datetime.now().strftime("Relatorio_Termico_%Y%m%d_%H%M%S_%f")
        self.exporting = True
        self.status.set("Gerando PDF, HTML, gráficos e dados analisados…")
        def worker():
            try:
                self.messages.put((True, export_bundle(folder, results, conditions)))
            except Exception as exc:
                self.messages.put((False, str(exc)))
        threading.Thread(target=worker, daemon=True).start()

    def poll_export(self):
        try:
            success, result = self.messages.get_nowait()
            self.exporting = False
            self.status.set("Relatório salvo em: " + str(result) if success else "Falha na exportação: " + str(result))
            if success:
                messagebox.showinfo("Relatório pronto", f"Arquivos salvos em:\n{result}\n\nPDF, HTML, gráficos PNG/SVG, dados CSV e resumo JSON.")
            else:
                messagebox.showerror("Exportação incompleta", str(result))
        except queue.Empty:
            pass
        self.after(150, self.poll_export)
