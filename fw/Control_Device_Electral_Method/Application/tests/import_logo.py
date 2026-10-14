from pathlib import Path
import sys
root = Path(__file__).resolve().parents[1]
source = Path(sys.argv[1])
data = source.read_bytes()
assert len(data) == 1024
lines = ['#ifndef THERMAL_LOGO_H', '#define THERMAL_LOGO_H', '#include <stdint.h>',
         '/* User-provided epd_bitmap_.bin: 128x64, XBM rows, LSB-first. */',
         '#define SPLASH_CUSTOM_BITMAP 1', '#define logo_width 128',
         '#define logo_height 64', 'static const uint8_t logo_bits[1024] = {']
lines += ['    '+','.join('0x%02X' % b for b in data[i:i+16])+','
          for i in range(0,len(data),16)]
lines += ['};','#endif','']
(root/'ui/logo.h').write_text('\n'.join(lines),encoding='utf-8')
(root/'ui/epd_bitmap_.bin').write_bytes(data)
print('Original bitmap embedded without alteration')
