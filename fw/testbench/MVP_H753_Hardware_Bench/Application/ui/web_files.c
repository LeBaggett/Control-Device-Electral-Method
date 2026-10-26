#include "lwip/apps/fs.h"
#include "lwip/mem.h"
#include "bench_diag.h"
#include "index_html.h"
#include <stdio.h>
#include <string.h>
int fs_open_custom(struct fs_file *file,const char *name) {
 memset(file,0,sizeof(*file));
 if(strcmp(name,"/index.html")==0 || strcmp(name,"/")==0) {
  g_bench_debug.http_requests++;
  file->data=(const char *)bench_index_http;file->len=sizeof(bench_index_http);
 } else if(strcmp(name,"/api/status")==0) {
  g_bench_debug.http_requests++;
  char body[1024];int length=bench_diag_json(body,sizeof(body));
  if(length<0)return 0;
  char *data=mem_malloc(1536);
  if(!data){g_bench_debug.http_alloc_failures++;return 0;}
  int n=snprintf(data,1536,"HTTP/1.0 200 OK\r\nContent-Type: application/json\r\nCache-Control: no-store\r\nConnection: close\r\nContent-Length: %d\r\n\r\n%s",length,body);
  if(n<0 || n>=1536){mem_free(data);return 0;}
  file->data=data;file->len=n;file->pextension=data;
 } else return 0;
 file->index=file->len;file->flags=FS_FILE_FLAGS_HEADER_INCLUDED;
 return 1;
}
void fs_close_custom(struct fs_file *file) {
 if(file->pextension)mem_free(file->pextension);
 file->pextension=0;
}
