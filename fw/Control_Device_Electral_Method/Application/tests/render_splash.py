from pathlib import Path
import struct
import zlib
import sys
root = Path(__file__).resolve().parents[1]
data = Path(sys.argv[1]).read_bytes()
assert len(data) == 1024, 'Expected 128x64 monochrome, 1024 bytes'
out = root / 'preview'
out.mkdir(exist_ok=True)
def chunk(kind, payload):
    return (struct.pack('>I',len(payload)) + kind + payload +
            struct.pack('>I',zlib.crc32(kind+payload)&0xffffffff))
for mode in ('row_msb','row_lsb','page_lsb','page_msb'):
    pixels = []
    for y in range(64):
        row = []
        for x in range(128):
            if mode.startswith('row'):
                index = y*16 + x//8
                bit = 7-x%8 if mode == 'row_msb' else x%8
            else:
                index = (y//8)*128 + x
                bit = y%8 if mode == 'page_lsb' else 7-y%8
            row.append(255 if data[index] & (1<<bit) else 0)
        pixels.append(row)
    raw = b''.join(bytes([0]) + bytes(v for v in row for _ in range(4))
                   for row in pixels for _ in range(4))
    png = (bytes([137,80,78,71,13,10,26,10]) +
           chunk(b'IHDR',struct.pack('>IIBBBBB',512,256,8,0,0,0,0)) +
           chunk(b'IDAT',zlib.compress(raw)) + chunk(b'IEND',b''))
    (out/(mode+'.png')).write_bytes(png)
print(out.resolve())
