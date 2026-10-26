#ifndef MOCK_FATFS_H
#define MOCK_FATFS_H
#include <stddef.h>
typedef unsigned UINT;
typedef unsigned char BYTE;
typedef int FRESULT;
typedef struct { int reserved; unsigned fs_type; } FATFS;
typedef struct { int index; size_t pos; } FIL;
#define FR_OK 0
#define FR_EXIST 1
#define FR_NO_FILE 2
#define FR_DISK_ERR 3
#define FA_READ 1U
#define FA_WRITE 2U
#define FA_CREATE_NEW 4U
#define FA_CREATE_ALWAYS 8U
FRESULT f_mount(FATFS*,const char*,BYTE);
FRESULT f_open(FIL*,const char*,BYTE);
FRESULT f_write(FIL*,const void*,UINT,UINT*);
FRESULT f_read(FIL*,void*,UINT,UINT*);
FRESULT f_sync(FIL*);
FRESULT f_close(FIL*);
FRESULT f_unlink(const char*);
FRESULT f_rename(const char*,const char*);
#endif
