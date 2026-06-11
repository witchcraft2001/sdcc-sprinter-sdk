/*
 * win0_io.c - pointer-marshalling wrappers (Phase 3). See win0_io.h.
 *
 * The staging buffer lives in the payload's _BSS, which is linked into WIN2
 * (>=0x4000) -- so it is a valid pointer to hand to DSS while WIN0 holds the
 * DSS core page. Input pointers are copied into it before the call; output
 * pointers are filled by the call and copied back afterwards (when our code
 * page is back in WIN0). The actual firmware call goes through the standard
 * dss_* wrappers, whose RST is routed by our WIN0 trampolines.
 */
#include <sprinter.h>
#include <string.h>
#include "win0_io.h"

#define WIN0_LIMIT 0x4000
#define IN_WIN0(p) (((u16)(p)) < WIN0_LIMIT)
#define STAGE_SZ   256

static char w_stage[STAGE_SZ];      /* in _BSS -> WIN2 */

void w_puts(const char *s) {
    u16 i;
    if (!IN_WIN0(s)) { dss_puts(s); return; }
    for (;;) {
        i = 0;
        while (i < (STAGE_SZ - 1) && s[i]) { w_stage[i] = s[i]; i++; }
        w_stage[i] = 0;
        dss_puts(w_stage);
        if (!s[i]) break;
        s += i;
    }
}

void w_puthex(u16 v) {
    static const char hexd[] = "0123456789ABCDEF";
    char b[5];
    b[0] = hexd[(v >> 12) & 15];
    b[1] = hexd[(v >> 8) & 15];
    b[2] = hexd[(v >> 4) & 15];
    b[3] = hexd[v & 15];
    b[4] = 0;
    w_puts(b);                        /* b is on the stack (WIN2) */
}

u8 w_curdir(char *buf) {
    u8 r;
    if (!IN_WIN0(buf)) return dss_curdir(buf);
    r = dss_curdir(w_stage);
    strcpy(buf, w_stage);             /* copy back after the call */
    return r;
}

i16 w_creat(const char *path) {
    if (!IN_WIN0(path)) return dss_creat(path);
    strncpy(w_stage, path, STAGE_SZ - 1);
    w_stage[STAGE_SZ - 1] = 0;
    return dss_creat(w_stage);
}

i16 w_open(const char *path, u8 mode) {
    if (!IN_WIN0(path)) return dss_open(path, mode);
    strncpy(w_stage, path, STAGE_SZ - 1);
    w_stage[STAGE_SZ - 1] = 0;
    return dss_open(w_stage, mode);
}

i16 w_read(u8 fd, void *buf, u16 count) {
    u16 total, chunk;
    i16 n;
    u8 *p;
    if (!IN_WIN0(buf)) return dss_read(fd, buf, count);
    p = (u8 *)buf;
    total = 0;
    while (count) {
        chunk = (count > STAGE_SZ) ? STAGE_SZ : count;
        n = dss_read(fd, w_stage, chunk);
        if (n <= 0) break;
        memcpy(p, w_stage, (u16)n);   /* WIN0 dest, WIN2 src; no DSS call here */
        p += n; total += n; count -= n;
        if ((u16)n < chunk) break;    /* short read = EOF */
    }
    return (i16)total;
}

i16 w_write(u8 fd, const void *buf, u16 count) {
    u16 total, chunk;
    i16 n;
    const u8 *p;
    if (!IN_WIN0(buf)) return dss_write(fd, buf, count);
    p = (const u8 *)buf;
    total = 0;
    while (count) {
        chunk = (count > STAGE_SZ) ? STAGE_SZ : count;
        memcpy(w_stage, p, chunk);    /* WIN0 src -> WIN2 stage; no DSS call */
        n = dss_write(fd, w_stage, chunk);
        if (n <= 0) break;
        p += n; total += n; count -= n;
        if ((u16)n < chunk) break;
    }
    return (i16)total;
}
