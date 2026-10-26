from pathlib import Path
root=Path(__file__).resolve().parents[1]
body=(root/'Application/ui/index.html').read_bytes()
response=b'HTTP/1.0 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nCache-Control: no-store\r\nConnection: close\r\nContent-Length: '+str(len(body)).encode()+b'\r\n\r\n'+body
header='/* Generated from index.html by tools/embed_web.py. */\nstatic const unsigned char bench_index_http[]={\n'+',\n'.join(','.join(str(v) for v in response[i:i+24]) for i in range(0,len(response),24))+'\n};\n'
(root/'Application/ui/index_html.h').write_text(header,encoding='utf-8')
print('Embedded page updated. Rebuild before flashing.')
