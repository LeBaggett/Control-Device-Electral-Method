#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/stat.h>
/* No filesystem/console in stage 1. Fail unsupported libc I/O explicitly. */
int _close(int fd){(void)fd;errno=EBADF;return -1;}
int _lseek(int fd,int off,int how){(void)fd;(void)off;(void)how;errno=ESPIPE;return -1;}
int _read(int fd,char *buf,int n){(void)fd;(void)buf;(void)n;errno=ENOSYS;return -1;}
int _write(int fd,const char *buf,int n){(void)fd;(void)buf;(void)n;errno=ENOSYS;return -1;}
int _fstat(int fd,struct stat *st){(void)fd;st->st_mode=S_IFCHR;return 0;}
int _isatty(int fd){(void)fd;return 0;}
int _getpid(void){return 1;}
int _kill(int pid,int sig){(void)pid;(void)sig;errno=EINVAL;return -1;}
void _exit(int code){(void)code;for(;;){} }
void *_sbrk(ptrdiff_t increment){
 extern char _end,_estack;static uintptr_t current;
 if(!current)current=(uintptr_t)&_end;
 uintptr_t base=(uintptr_t)&_end,limit=(uintptr_t)&_estack-0x2000U;
 if((increment>=0 && (uintptr_t)increment>limit-current) ||
    (increment<0 && (uintptr_t)(-increment)>current-base)){errno=ENOMEM;return (void*)-1;}
 uintptr_t old=current;current+=increment;return (void*)old;
}
