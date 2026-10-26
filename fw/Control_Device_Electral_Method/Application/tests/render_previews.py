from pathlib import Path
import struct
import zlib
root=Path(__file__).resolve().parents[1]
def chunk(kind,payload):
    return struct.pack('>I',len(payload))+kind+payload+struct.pack('>I',zlib.crc32(kind+payload)&0xffffffff)
for name in ('home','menu','setup','confirm','running','error','result'):
    data=(root/'preview'/f'{name}.bin').read_bytes()
    rows=[]
    for y in range(64):
        row=bytes(255 if data[y*16+x//8]&(1<<(x%8)) else 0 for x in range(128))
        rows.extend([bytes([0])+bytes(v for v in row for _ in range(4))]*4)
    png=bytes([137,80,78,71,13,10,26,10])+chunk(b'IHDR',struct.pack('>IIBBBBB',512,256,8,0,0,0,0))
    png+=chunk(b'IDAT',zlib.compress(b''.join(rows)))+chunk(b'IEND',b'')
    (root/'preview'/f'{name}.png').write_bytes(png)
print('Screen previews rendered from actual C framebuffer')
