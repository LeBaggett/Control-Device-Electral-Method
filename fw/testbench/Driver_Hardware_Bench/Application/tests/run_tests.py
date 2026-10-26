from pathlib import Path
import subprocess
import tempfile
root=Path(__file__).resolve().parents[1]
stubs=root/'tests/stubs'
with tempfile.TemporaryDirectory(prefix='bench-test-') as temp:
    exe=Path(temp)/'bench.exe'
    args=['gcc','-std=c11','-Wall','-Wextra','-Werror']
    args+=['-I'+str(root/p) for p in ['.','board','drivers','core','interfaces']]
    args += [str(root/'drivers/hw_debug.c')]
    args+=[str(root/p) for p in ['tests/bench_test.c','app.c','core/format.c']]
    subprocess.run(args+['-lm','-o',str(exe)],check=True)
    subprocess.run([str(exe)],check=True)
    probe=Path(temp)/'sdprobe.exe'
    subprocess.run(['gcc','-std=c11','-Wall','-Wextra','-Werror',
        '-I'+str(stubs),'-I'+str(root/'drivers'),str(root/'tests/sdprobe_test.c'),
        str(root/'drivers/sdprobe.c'),'-o',str(probe)],check=True)
    subprocess.run([str(probe)],check=True)
    led=Path(temp)/'led.exe'
    subprocess.run(['gcc','-std=c11','-Wall','-Wextra','-Werror',
        '-I'+str(stubs),'-I'+str(root),'-I'+str(root/'board'),
        str(root/'tests/led_test.c'),str(root/'board/bench_led.c'),
        '-o',str(led)],check=True)
    subprocess.run([str(led)],check=True)
with tempfile.TemporaryDirectory(prefix='oled-test-') as temp:
    exe=Path(temp)/'oled.exe'
    subprocess.run(['gcc','-std=c11','-Wall','-Wextra','-Werror','-I'+str(root),'-I'+str(root/'board'),'-I'+str(root/'drivers'),str(root/'tests/oled_profiles_test.c'),str(root/'drivers/oled.c'),'-o',str(exe)],check=True)
    subprocess.run([str(exe)],check=True)
    print('SSD1306 / SH1106 protocol and failure tests: PASS')
with tempfile.TemporaryDirectory(prefix='input-test-') as temp:
    exe=Path(temp)/'input.exe'
    args=['gcc','-std=c11','-Wall','-Wextra','-Werror'] + ['-I'+str(root/p) for p in ['.','board','drivers','interfaces']]
    args += [str(root/p) for p in ['tests/input_temperature_test.c','drivers/encoder.c','drivers/max31855.c','drivers/hw_debug.c']]
    subprocess.run(args+['-lm','-o',str(exe)],check=True)
    subprocess.run([str(exe)],check=True)
with tempfile.TemporaryDirectory(prefix='oled-boot-') as temp:
    exe=Path(temp)/'boot.exe'
    args=['gcc','-std=c11','-Wall','-Wextra','-Werror'] + ['-I'+str(root/p) for p in ['.','board','drivers']]
    args += [str(root/p) for p in ['tests/oled_boot_test.c','drivers/oled.c']]
    subprocess.run(args+['-o',str(exe)],check=True)
    subprocess.run([str(exe)],check=True)
    print('OLED delayed readiness, recovery and retry cap: PASS')
print('Host software test only; no physical hardware validation.')


with tempfile.TemporaryDirectory(prefix='manual-board-') as temp:
    exe=Path(temp)/'board.exe'
    args=['gcc','-std=c11','-Wall','-Wextra','-Werror','-DBOARD_POWER_CONTROL_ENABLED=1']
    args+=['-I'+str(root/p) for p in ['tests/stubs','board','drivers','.']]
    args+=[str(root/p) for p in ['tests/manual_board_test.c','board/board.c','drivers/power.c']]
    subprocess.run(args+['-o',str(exe)],check=True)
    subprocess.run([str(exe)],check=True)

with tempfile.TemporaryDirectory(prefix='sd-sector-trace-') as temp:
    exe=Path(temp)/'trace.exe'
    subprocess.run(['gcc','-std=c11','-Wall','-Wextra','-Werror','-I'+str(root/'board'),str(root/'tests/sd_sector_trace_test.c'),'-o',str(exe)],check=True)
    subprocess.run([str(exe)],check=True)

# Real filesystem integration: temporary images only, never physical cards.
import sys
subprocess.run([sys.executable,str(root/"tests/run_filesystems_test.py")],check=True)
