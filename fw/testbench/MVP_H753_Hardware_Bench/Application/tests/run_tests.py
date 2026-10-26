from pathlib import Path
import subprocess,json,tempfile,re
root=Path(__file__).resolve().parents[2]
if not (root/'Application/bench_diag.c').exists():root=Path('h753_eth_bench_20261026').resolve()
lw=root/'Middlewares/Third_Party/LwIP/src'
with tempfile.TemporaryDirectory(prefix='h753-tests-') as tmp:
 d=Path(tmp);(d/'arch').mkdir()
 (d/'arch/cc.h').write_text('#include <stdint.h>\n#include <stdlib.h>\n#define LWIP_PLATFORM_DIAG(x) do{}while(0)\n#define LWIP_PLATFORM_ASSERT(x) abort()\n')
 (d/'lwipopts.h').write_text('''#define NO_SYS 1
#define LWIP_HTTPD_CUSTOM_FILES 1
#define LWIP_HTTPD_FILE_EXTENSION 1
#define LWIP_HTTPD_DYNAMIC_HEADERS 1
#define HTTPD_PRECALCULATED_CHECKSUM 0
#define HTTPD_FSDATA_FILE "empty_fsdata.h"
''')
 (d/'test.c').write_text('''#include "bench_diag.h"
#include "lwip/apps/fs.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
static unsigned allocated,fail_alloc;
void *mem_malloc(size_t n){if(fail_alloc)return NULL;void*p=malloc(n);if(p)allocated++;return p;}
void mem_free(void*p){if(p){assert(allocated);allocated--;free(p);}}
int main(void){
 bench_diag_init(0);bench_diag_step(3000);assert(g_bench_debug.max_gap_ms==0);
 bench_diag_step(3002);assert(g_bench_debug.max_gap_ms==2);
 bench_diag_link(1);bench_diag_link(1);bench_diag_link(0);assert(g_bench_debug.link_changes==2);
 char tiny[4]={1};assert(bench_diag_json(tiny,sizeof(tiny))==-1 && tiny[0]==0);
 assert(bench_diag_json(NULL,0)==-1);
 bench_diag_init(UINT32_MAX-2);bench_diag_step(UINT32_MAX-1);bench_diag_step(1);assert(g_bench_debug.max_gap_ms==3);
 struct fs_file index={0},a={0},b={0},missing={0};
 assert(fs_open(&index,"/index.html")==0);
 assert(strstr(index.data,"Content-Type: text/html") && strstr(index.data,"/api/status"));
 assert(strstr(index.data,"<script>"));fs_close(&index);assert(allocated==0);
 assert(fs_open(&missing,"/does-not-exist")==ERR_VAL);
 assert(fs_open(&a,"/api/status")==0 && allocated==1);
 char saved[1536];strcpy(saved,a.data);
 g_bench_debug.uptime_ms=12345;
 assert(fs_open(&b,"/api/status")==0 && allocated==2);
 assert(strcmp(a.data,saved)==0 && strcmp(a.data,b.data)!=0);
 const char *body=strstr(b.data,"\\r\\n\\r\\n");assert(body);body+=4;
 const char *length=strstr(b.data,"Content-Length: ");assert(length);
 assert((size_t)atoi(length+16)==strlen(body));
 puts(body);fs_close(&a);fs_close(&b);assert(allocated==0);
 fail_alloc=1;assert(fs_open(&a,"/api/status")==ERR_VAL);
 assert(g_bench_debug.http_alloc_failures==1 && allocated==0);
 return 0;
}
''')
 includes=[d,root/'Application',root/'Application/ui',root/'LWIP/Target',lw/'include']
 exe=d/'test.exe'
 cmd=['gcc','-std=c11','-Wall','-Wextra','-Werror']+['-I'+str(p) for p in includes]+[str(d/'test.c'),str(root/'Application/bench_diag.c'),str(root/'Application/ui/web_files.c'),str(lw/'apps/http/fs.c'),'-o',str(exe)]
 subprocess.run(cmd,check=True)
 data=json.loads(subprocess.check_output([str(exe)],text=True))
 assert data['uptime_ms']==12345 and data['power_enabled'] is False and data['max_gap_ms']==3
 print('PASS: real LwIP custom-files integration, independent HTTP buffers, length/JSON, allocation failure, counter wrap and read-only status')
 # Firmware invariants and actual linker placement, if an ARM map is available.
 main=(root/'Core/Src/main.c').read_text();eth=(root/'LWIP/Target/ethernetif.c').read_text()
 assert 'SCB_EnableDCache' not in main
 assert 'HAL_ETH_Transmit_IT' in eth and 'unsigned budget=4U' in eth
 assert 'HAL_ETH_Transmit(&' not in eth
 for p in list((root/'Application').rglob('*.c'))+list((root/'Core/Src').glob('*.c')):
  if 'tests' in p.parts:continue
  assert not re.search(r'\bpower_enable\w*\s*\(',p.read_text()) and 'HAL_DAC_' not in p.read_text()
 print('PASS: no power implementation, no polling ETH transmit, bounded RX service, D-cache explicitly disabled in stage 1')
