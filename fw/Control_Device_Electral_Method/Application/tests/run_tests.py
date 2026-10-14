"""Host regression and HAL-shim checks. Requires gcc on PATH; no target build."""
from pathlib import Path
import subprocess
import tempfile
import sys
root = Path(__file__).resolve().parents[1]
includes = ['board', 'core', 'drivers', 'ui', 'interfaces', '.']
common = ['gcc', '-std=c11', '-Wall', '-Wextra', '-Werror']
common += ['-I' + str(root / p) for p in includes]
cases = {
    'contract': ['tests/contract_check.c'],
    'job': ['tests/job_test.c', 'core/job.c','core/targets.c','core/linfit.c','core/format.c'],
    'power_max31855': ['tests/power_max31855_test.c', 'drivers/power.c', 'drivers/max31855.c'],
    'board': ['tests/board_integration_test.c', 'board/board.c', 'drivers/power.c'],
    'dwt': ['tests/dwt_test.c', 'board/dwt_us.c'],
    'math': ['tests/math_test.c','core/targets.c','core/linfit.c','core/vf_math.c','core/format.c'],
    'ui': ['tests/ui_driver_test.c','ui/ui.c','ui/assets.c','drivers/oled.c','drivers/encoder.c','core/format.c'],
    'adc': ['tests/adc_test.c','drivers/vf_adc.c','core/vf_math.c'],
    'sdlog': ['tests/sdlog_test.c','drivers/sdlog.c','core/format.c'],
    'sd_board': ['tests/sd_board_test.c','board/sd_io.c'],
    'app': ['tests/app_test.c','app.c','ui/ui.c','ui/assets.c',
        'drivers/oled.c','drivers/encoder.c','drivers/vf_adc.c','drivers/max31855.c',
        'drivers/rtd.c','drivers/power.c','drivers/sdlog.c','core/job.c',
        'core/targets.c','core/linfit.c','core/vf_math.c','core/format.c','core/calibration.c'],
    'require_cal': ['tests/require_cal_test.c','core/job.c','core/targets.c',
        'core/linfit.c','core/format.c','ui/ui.c','ui/assets.c'],
}
with tempfile.TemporaryDirectory(prefix='thermal-tests-') as temp:
    selected = sys.argv[1:] or list(cases)
    for name in selected:
        sources = cases[name]
        cmd = common.copy()
        if name in ('board', 'dwt', 'sdlog', 'sd_board'):
            cmd += ['-I' + str(root / 'tests/stubs')]
        if name == 'job':
            cmd += ['-DCAL_SOAK_S=2','-DCAL_WAIT_TIMEOUT_S=20','-DTEST_TARGET_TIMEOUT_S=20']
        if name == 'sdlog':
            cmd += ['-DTHERMAL_USE_FATFS=1']
        if name == 'app':
            cmd += ['-DTHERMAL_USE_FATFS=0']
        if name == 'require_cal':
            cmd += ['-DREQUIRE_CAL=1']
        exe = Path(temp) / (name + '.exe')
        subprocess.run(cmd + [str(root / p) for p in sources] + ['-lm', '-o', str(exe)], check=True)
        if name == 'ui':
            preview=root/'preview';preview.mkdir(exist_ok=True)
            subprocess.run([str(exe),str(preview)],check=True)
        else:
            subprocess.run([str(exe)], check=True)
        print(name + ': PASS', flush=True)
    # Real HAL adapter is compiled against the shim, never called with fake MCU addresses.
    subprocess.run(common + ['-I'+str(root/'tests/stubs'),'-c',
        str(root/'board/peripheral_io.c'),'-o',str(Path(temp)/'peripheral_io.o')],check=True)
    print('HAL peripheral adapter syntax: PASS',flush=True)
print('All host checks passed. No real STM32 HAL build or hardware validation.')
