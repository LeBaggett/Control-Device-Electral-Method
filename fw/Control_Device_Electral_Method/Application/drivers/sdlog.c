#include "sdlog.h"
#include "runtime_config.h"
#include "format.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sd_io.h"
static bool mounted, open_run;
static char run_name[32];
bool sdlog_ready(void)
{
#if THERMAL_USE_FATFS
    if (mounted && !board_sd_is_ready()) mounted = false;
#endif
    return mounted;
}
const char *sdlog_run_name(void) { return run_name; }
#if THERMAL_USE_FATFS
#include "ff.h"
static FATFS fs;
static FIL run;
static bool write_exact(FIL *file, const char *text)
{
    UINT written = 0;
    size_t length = strlen(text);
    return length <= 512U && f_write(file,text,(UINT)length,&written) == FR_OK &&
        written == length && f_sync(file) == FR_OK;
}
static bool storage_error(void)
{
    mounted = false;
    if (open_run) { (void)f_close(&run); open_run = false; }
    return false;
}
void sdlog_init(void)
{
    open_run = false; run_name[0] = 0;
    mounted = board_sd_is_ready() && f_mount(&fs,THERMAL_SD_VOLUME,1) == FR_OK;
}
bool sdlog_open_run(bool calibration)
{
    if (!mounted || open_run) return false;
    for (unsigned index = 1; index <= 999; ++index) {
        (void)snprintf(run_name,sizeof(run_name),"%s/%s_%03u.CSV",THERMAL_SD_VOLUME,
                       calibration?"CAL":"TST",index);
        FRESULT result = f_open(&run,run_name,FA_WRITE|FA_CREATE_NEW);
        if (result == FR_EXIST) continue;
        if (result != FR_OK) return storage_error();
        open_run = true;
        const char *header = calibration ? "idx,t_alvo_c,t_rtd_c,vf_v\r\n" :
            "idx,t_alvo_c,t_rtd_c,vf_v,tj_c,t_s\r\n";
        if (!write_exact(&run,header)) return storage_error();
        return true;
    }
    run_name[0] = 0; return false; /* No free slot; never overwrite a run. */
}
bool sdlog_line(const char *line)
{
    if (!mounted || !open_run || !line || strlen(line) > 250U) return false;
    char buffer[256];
    int n = snprintf(buffer,sizeof(buffer),"%s\r\n",line);
    if (n < 0 || (size_t)n >= sizeof(buffer)) return false;
    if (!write_exact(&run,buffer)) return storage_error();
    return true;
}
bool sdlog_close(void)
{
    if (!open_run) return mounted;
    bool synced = f_sync(&run) == FR_OK;
    bool closed = f_close(&run) == FR_OK;
    open_run = false;
    if (!synced || !closed) mounted = false;
    return synced && closed;
}
static bool valid_cal(const cal_params_t *c)
{
    return c && c->valid && c->n >= 2 && c->n <= TARGETS_MAX && isfinite(c->m) &&
        fabsf(c->m) > 0.000001f && isfinite(c->t0) && isfinite(c->r2) &&
        c->r2 >= 0 && c->r2 <= 1;
}
static bool parse_cal(char *text, cal_params_t *out)
{
    const char *header = "m,t0,r2,n\r\n";
    if (strncmp(text,header,strlen(header)) != 0) return false;
    char *p = text+strlen(header), *end;
    cal_params_t candidate = {0};
    candidate.m = strtof(p,&end); if (end == p || *end++ != ',') return false; p=end;
    candidate.t0 = strtof(p,&end); if (end == p || *end++ != ',') return false; p=end;
    candidate.r2 = strtof(p,&end); if (end == p || *end++ != ',') return false; p=end;
    unsigned long n = strtoul(p,&end,10);
    if (end == p || n > UINT16_MAX || strncmp(end,"\r\n#crc=",7) != 0) return false;
    char *crc_start = end+2;
    unsigned long stored_crc = strtoul(end+7,&end,16);
    if (strcmp(end,"\r\n") != 0 || stored_crc > UINT32_MAX ||
        thermal_crc32((const uint8_t*)text,(size_t)(crc_start-text)) != stored_crc)
        return false;
    candidate.n = (uint16_t)n; candidate.valid = true;
    if (!valid_cal(&candidate)) return false;
    *out = candidate; return true;
}
static bool load_file(const char *name, cal_params_t *out)
{
    FIL file; char text[160]; UINT bytes = 0;
    if (f_open(&file,name,FA_READ) != FR_OK) return false;
    FRESULT result = f_read(&file,text,sizeof(text)-1,&bytes);
    bool closed = f_close(&file) == FR_OK;
    if (result != FR_OK || !closed || bytes >= sizeof(text)-1 ||
        memchr(text,0,bytes) != NULL) return false;
    text[bytes] = 0; return parse_cal(text,out);
}
bool sdlog_load_calibration(cal_params_t *out)
{
    if (!out) return false;
    *out = (cal_params_t){0};
    if (!mounted) return false;
    return load_file(THERMAL_SD_VOLUME "/CALIB.TXT",out) ||
           load_file(THERMAL_SD_VOLUME "/CALIB.BAK",out);
}
bool sdlog_save_calibration(const cal_params_t *c)
{
    if (!mounted || open_run || !valid_cal(c)) return false;
    char m[24], t0[24], r2[24], text[160];
    if (!format_decimal(m,sizeof(m),c->m,3) || !format_decimal(t0,sizeof(t0),c->t0,4) ||
        !format_decimal(r2,sizeof(r2),c->r2,6)) return false;
    int n = snprintf(text,sizeof(text),"m,t0,r2,n\r\n%s,%s,%s,%u\r\n",m,t0,r2,c->n);
    if (n < 0 || (size_t)n+18 >= sizeof(text)) return false;
    uint32_t crc = thermal_crc32((const uint8_t *)text,(size_t)n);
    (void)snprintf(text+n,sizeof(text)-(size_t)n,"#crc=%08lX\r\n",(unsigned long)crc);
    FIL temporary;
    if (f_open(&temporary,THERMAL_SD_VOLUME "/CALIB.TMP",FA_WRITE|FA_CREATE_ALWAYS) != FR_OK)
        return storage_error();
    bool written = write_exact(&temporary,text);
    bool closed = f_close(&temporary) == FR_OK;
    if (!written || !closed) return storage_error();
    cal_params_t verified;
    if (!load_file(THERMAL_SD_VOLUME "/CALIB.TMP",&verified)) return storage_error();
    /* Preserve previous valid calibration as backup before replacing it.
     * FAT rename is not an atomic power-loss transaction; test on actual card. */
    FRESULT result;
    if (load_file(THERMAL_SD_VOLUME "/CALIB.TXT",&verified)) {
        result = f_unlink(THERMAL_SD_VOLUME "/CALIB.BAK");
        if (result != FR_OK && result != FR_NO_FILE) return storage_error();
        result = f_rename(THERMAL_SD_VOLUME "/CALIB.TXT",THERMAL_SD_VOLUME "/CALIB.BAK");
        if (result != FR_OK) return storage_error();
    } else {
        /* If primary is corrupt/absent, retain existing valid backup. */
        result = f_unlink(THERMAL_SD_VOLUME "/CALIB.TXT");
        if (result != FR_OK && result != FR_NO_FILE) return storage_error();
    }
    if (f_rename(THERMAL_SD_VOLUME "/CALIB.TMP",THERMAL_SD_VOLUME "/CALIB.TXT") != FR_OK)
        return storage_error();
    return true;
}
#else
/* No fabricated successful storage when CubeMX FatFs is not integrated. */
void sdlog_init(void) { mounted = open_run = false; run_name[0] = 0; }
bool sdlog_open_run(bool calibration) { (void)calibration; return false; }
bool sdlog_line(const char *line) { (void)line; return false; }
bool sdlog_close(void) { return false; }
bool sdlog_save_calibration(const cal_params_t *cal) { (void)cal; return false; }
bool sdlog_load_calibration(cal_params_t *cal)
{
    if (cal) *cal = (cal_params_t){0};
    return false;
}
#endif
