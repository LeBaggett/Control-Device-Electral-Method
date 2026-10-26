import math
from pathlib import Path
import tempfile
import unittest
import zlib
from thermal_report.data import read_run, read_calibration
from thermal_report.analysis import analyze, fit_calibration, Conditions


class AnalysisTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.folder = Path(self.temp.name)

    def tearDown(self):
        self.temp.cleanup()

    def run_file(self, text, name="CAL_001.CSV"):
        p = self.folder/name
        p.write_text(text, encoding="utf-8")
        return read_run(p)

    def test_fit_matches_firmware_orientation(self):
        run = self.run_file("idx,t_alvo_c,t_rtd_c,vf_v\n0,25,25,.65\n1,50,50,.60\n2,75,75,.55\n")
        fit = fit_calibration(run)
        self.assertAlmostEqual(fit.slope, -500)
        self.assertAlmostEqual(fit.intercept, 350)
        self.assertAlmostEqual(fit.r2, 1)
        self.assertAlmostEqual(fit.temperature(.58), 60)

    def test_invalid_lines_and_decimal_comma(self):
        run = self.run_file("\ufeffidx;t_alvo_c;t_rtd_c;vf_v\n0;25;25,1;0,650\n1;nan;50;0,6\n2;75;75\n")
        self.assertEqual(len(run.samples), 1)
        self.assertAlmostEqual(run.samples[0].reference, 25.1)
        self.assertEqual(run.skipped, 2)

    def test_header_only_and_constant_calibration(self):
        run = self.run_file("idx,t_alvo_c,t_rtd_c,vf_v\n")
        self.assertEqual(analyze(run).metrics, {})
        run = self.run_file("idx,t_alvo_c,t_rtd_c,vf_v\n0,25,25,.6\n1,50,50,.6\n")
        self.assertIsNone(analyze(run).calibration)

    def test_no_automatic_calibration_pairing_or_fake_missing_tj(self):
        run = self.run_file("idx,t_alvo_c,t_rtd_c,vf_v,tj_c,t_s\n0,25,25,.65,NaN,0\n1,50,50,.60,52,10\n", "TST_001.CSV")
        result = analyze(run)
        self.assertIsNone(result.junction[0])
        self.assertEqual(result.junction[1], 52)
        self.assertNotIn("Rθj-a estimada no ponto final (°C/W)", result.metrics)
        conditions = Conditions(ambient=25, power=2, steady=True)
        self.assertAlmostEqual(analyze(run, conditions=conditions).metrics["Rθj-a estimada no ponto final (°C/W)"], 13.5)
        with self.assertRaises(ValueError):
            analyze(run, conditions=Conditions(power=0))

    def test_crc_and_tc_metadata(self):
        payload = b"m,t0,r2,n\r\n-500,350,1,3\r\n#tc=1.000000,0.0000\r\n"
        p = self.folder/"CALIB.TXT"
        p.write_bytes(payload + f"#crc={zlib.crc32(payload):08X}\r\n".encode())
        model = read_calibration(p)
        self.assertEqual(model.temperature(.6), 50)
        self.assertEqual(model.tc_gain, 1)
        p.write_bytes(p.read_bytes().replace(b"-500", b"-501"))
        with self.assertRaises(ValueError):
            read_calibration(p)

    def test_explicit_model_marks_extrapolation(self):
        cal = self.run_file("idx,t_alvo_c,t_rtd_c,vf_v\n0,25,25,.65\n1,50,50,.6\n")
        run = self.run_file("idx,t_alvo_c,t_rtd_c,vf_v,tj_c,t_s\n0,75,75,.55,NaN,10\n", "TST_001.CSV")
        result = analyze(run, fit_calibration(cal))
        self.assertAlmostEqual(result.junction[0], 75)
        self.assertTrue(any("extrapolação" in s for s in result.warnings))
        self.assertTrue(any("manualmente" in s for s in result.warnings))


if __name__ == "__main__":
    unittest.main()
