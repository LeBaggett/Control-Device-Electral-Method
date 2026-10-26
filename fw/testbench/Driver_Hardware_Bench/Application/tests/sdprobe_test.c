#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "ff.h"
#include "sdprobe.h"
static char stored[128];
static UINT length;
static bool exists, bad_read, short_write;
static unsigned created;
FRESULT f_open(FIL *file,const char *name,BYTE mode)
{
    (void)file;
    if (mode & FA_CREATE_NEW) {
        if (exists && strstr(name,"BCH001")) return FR_EXIST;
        assert(strstr(name,"BCH001") || strstr(name,"BCH002"));
        created++; exists=true;
    }
    return FR_OK;
}
FRESULT f_write(FIL *file,const void *p,UINT count,UINT *written)
{
    (void)file; assert(count<sizeof(stored));memcpy(stored,p,count);length=count;
    *written=short_write?count-1:count;return FR_OK;
}
FRESULT f_sync(FIL *file) { (void)file;return FR_OK; }
FRESULT f_close(FIL *file) { (void)file;return FR_OK; }
FRESULT f_read(FIL *file,void *p,UINT count,UINT *read)
{
    (void)file;assert(count==length);memcpy(p,stored,length);*read=length;
    if(bad_read)((char*)p)[0]^=1;
    return FR_OK;
}
int main(void)
{
    assert(sdprobe_roundtrip() && created==1 && strstr(sdprobe_file(),"BCH001"));
    assert(sdprobe_roundtrip() && created==2 && strstr(sdprobe_file(),"BCH002"));
    bad_read=true;assert(!sdprobe_roundtrip());bad_read=false;
    short_write=true;assert(!sdprobe_roundtrip());
    puts("SD probe create-new, readback and write/read errors: PASS");return 0;
}
