/*
 * win0_dss.c - pointer-marshalling DSS wrappers for the win0 layout.
 *
 * These DEFINE dss_puts/open/creat/read/write/curdir, so when linked (only in
 * the win0 layout) they OVERRIDE the library's raw versions -- the linker uses
 * an explicitly-supplied object before pulling a library module of the same
 * name. They gate on `ptr < 0x4000` (WIN0, where the DSS core page sits during
 * a call) and bounce such pointers through a WIN2 staging buffer, then call the
 * raw wrapper (dss_*_raw, dss_raw.s). Pointers in WIN1/WIN2/WIN3 pass straight
 * through. The normal memory layout never links this file, so it pays ZERO
 * overhead.
 *
 * This gives transparent correctness even for library-internal calls such as
 * fopen() -> dss_open() with a WIN0 filename literal.
 */
#include <sprinter.h>
#include <string.h>

extern void dss_puts_raw(const char *s);
extern i16  dss_open_raw(const char *path, u8 mode);
extern i16  dss_creat_raw(const char *path);
extern i16  dss_read_raw(u8 fd, void *buf, u16 count);
extern i16  dss_write_raw(u8 fd, const void *buf, u16 count);
extern u8   dss_curdir_raw(char *buf);

#define IN_WIN0(p) (((u16)(p)) < 0x4000)
#define STAGE_SZ   256

static char w_stage[STAGE_SZ];      /* in _BSS -> WIN2 */

void dss_puts(const char *s) {
    u16 i;
    if (!IN_WIN0(s)) { dss_puts_raw(s); return; }
    for (;;) {
        i = 0;
        while (i < (STAGE_SZ - 1) && s[i]) { w_stage[i] = s[i]; i++; }
        w_stage[i] = 0;
        dss_puts_raw(w_stage);
        if (!s[i]) break;
        s += i;
    }
}

i16 dss_open(const char *path, u8 mode) {
    if (!IN_WIN0(path)) return dss_open_raw(path, mode);
    strncpy(w_stage, path, STAGE_SZ - 1);
    w_stage[STAGE_SZ - 1] = 0;
    return dss_open_raw(w_stage, mode);
}

i16 dss_creat(const char *path) {
    if (!IN_WIN0(path)) return dss_creat_raw(path);
    strncpy(w_stage, path, STAGE_SZ - 1);
    w_stage[STAGE_SZ - 1] = 0;
    return dss_creat_raw(w_stage);
}

i16 dss_read(u8 fd, void *buf, u16 count) {
    u16 total, chunk;
    i16 n;
    u8 *p;
    if (!IN_WIN0(buf)) return dss_read_raw(fd, buf, count);
    p = (u8 *)buf;
    total = 0;
    while (count) {
        chunk = (count > STAGE_SZ) ? STAGE_SZ : count;
        n = dss_read_raw(fd, w_stage, chunk);
        if (n <= 0) break;
        memcpy(p, w_stage, (u16)n);
        p += n; total += n; count -= n;
        if ((u16)n < chunk) break;
    }
    return (i16)total;
}

i16 dss_write(u8 fd, const void *buf, u16 count) {
    u16 total, chunk;
    i16 n;
    const u8 *p;
    if (!IN_WIN0(buf)) return dss_write_raw(fd, buf, count);
    p = (const u8 *)buf;
    total = 0;
    while (count) {
        chunk = (count > STAGE_SZ) ? STAGE_SZ : count;
        memcpy(w_stage, p, chunk);
        n = dss_write_raw(fd, w_stage, chunk);
        if (n <= 0) break;
        p += n; total += n; count -= n;
        if ((u16)n < chunk) break;
    }
    return (i16)total;
}

u8 dss_curdir(char *buf) {
    u8 r;
    if (!IN_WIN0(buf)) return dss_curdir_raw(buf);
    r = dss_curdir_raw(w_stage);
    strcpy(buf, w_stage);
    return r;
}
