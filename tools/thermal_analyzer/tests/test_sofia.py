from pathlib import Path
import unittest,tempfile
from thermal_report.data import read_run,Calibration
from thermal_report.analysis import analyze,Conditions
class SofiaTests(unittest.TestCase):
 def run_data(self):return read_run(Path(__file__).resolve().parents[1]/"examples/TSP_DEMO.CSV")
 def test_metadata_timing_and_power(self):
  run=self.run_data();self.assertEqual(run.kind,"TSP");self.assertEqual(len(run.samples),14)
  self.assertEqual(run.metadata["current_heating_a"],"0.758000")
  out=analyze(run,conditions=Conditions(ambient=25,power=99,steady=True))
  self.assertAlmostEqual(out.metrics["Potência aquecimento Ih declarado × Vh medida (W)"],.7201)
  self.assertAlmostEqual(out.metrics["Rθj-a estimada, regime declarado (°C/W)"],30/.7201,places=3)
  self.assertAlmostEqual(out.metrics["Razão aparente (Tj-Tcase)/P do arranjo (°C/W)"],20/.7201,places=3)
 def test_no_static_result_without_equilibrium(self):
  out=analyze(self.run_data())
  self.assertFalse(any("Rθj-a" in key for key in out.metrics))
 def test_ignore_unrelated_calibration(self):
  out=analyze(self.run_data(),Calibration("wrong",-1,0,1,3))
  self.assertIsNone(out.calibration);self.assertEqual(out.junction[1],55)
 def test_incomplete_file(self):
  with tempfile.TemporaryDirectory() as d:
   p=Path(d)/"interrupted.csv";p.write_text("idx,t_alvo_c,t_rtd_c,vf_v,tj_c,t_s\n#mode=SOFIA_STEP\n",encoding="utf-8")
   out=analyze(read_run(p));self.assertEqual(out.run.kind,"TSP")
   self.assertTrue(any("incompleto" in w for w in out.warnings))

 def test_current_mismatch_refuses_power(self):
  run=self.run_data();run.metadata["current_heating_a"]="0.75"
  out=analyze(run,conditions=Conditions(ambient=25,steady=True))
  self.assertFalse(any("Rθj-a" in key for key in out.metrics))
  self.assertFalse(any("Potência aquecimento" in key for key in out.metrics))
