#include "sdprobe.h"
#include "ff.h"
#include <stdio.h>
#include <string.h>
static char filename[24];
const char *sdprobe_file(void) { return filename; }
bool sdprobe_roundtrip(void)
{
    static const char pattern[] = "STM32 DRIVER BENCH READ WRITE\r\n";
    FIL file;
    FRESULT result = FR_EXIST;
    for (unsigned i = 1; i <= 999 && result == FR_EXIST; ++i) {
        (void)snprintf(filename,sizeof(filename),"0:/BCH%03u.TXT",i);
        result = f_open(&file,filename,FA_WRITE|FA_CREATE_NEW);
    }
    if (result != FR_OK) return false;
    UINT bytes = 0;
    bool wrote = f_write(&file,pattern,sizeof(pattern)-1,&bytes) == FR_OK &&
                 bytes == sizeof(pattern)-1 && f_sync(&file) == FR_OK;
    bool closed = f_close(&file) == FR_OK;
    if (!wrote || !closed || f_open(&file,filename,FA_READ) != FR_OK) return false;
    char readback[sizeof(pattern)] = {0}; bytes = 0;
    bool read = f_read(&file,readback,sizeof(pattern)-1,&bytes) == FR_OK &&
                bytes == sizeof(pattern)-1 && memcmp(pattern,readback,sizeof(pattern)-1) == 0;
    closed = f_close(&file) == FR_OK;
    return read && closed;
}
