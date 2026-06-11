/*
 * win0_dss.c - pointer-marshalling DSS wrappers for the win0 layout.
 *
 * These DEFINE the DSS calls that hand a pointer to firmware *during* the RST,
 * so when linked (only in the win0 layout) they OVERRIDE the library's raw
 * versions (an explicitly-supplied object is used before a same-named library
 * module). They gate on `ptr < 0x4000` (WIN0, where the DSS core page sits
 * during a call) and bounce such pointers through WIN2 staging buffers, then
 * call the raw wrapper (dss_*_raw, dss_raw.c). Pointers in WIN1/WIN2/WIN3 pass
 * straight through. The normal layout never links this file -> ZERO overhead.
 *
 * Not wrapped (already win0-safe): functions that only fill a struct in the
 * wrapper after the RST and pass values to DSS (getdate/gettime/settime/
 * meminfo/waitkey_ex/scankey/testkey), and value-only calls (putchar, ...).
 *
 * Two staging areas: w_name for the primary input string, w_stage for an output
 * or secondary buffer, so two-pointer calls (rename, ffirst, getenv, expath)
 * can bounce both operands independently.
 */
#include <sprinter.h>
#include <string.h>

extern void dss_puts_raw(const char *s);
extern i16  dss_open_raw(const char *path, u8 mode);
extern i16  dss_creat_raw(const char *path);
extern i16  dss_read_raw(u8 fd, void *buf, u16 count);
extern i16  dss_write_raw(u8 fd, const void *buf, u16 count);
extern u8   dss_curdir_raw(char *buf);
extern u8   dss_delete_raw(const char *path);
extern u8   dss_chdir_raw(const char *path);
extern u8   dss_mkdir_raw(const char *path);
extern u8   dss_rmdir_raw(const char *path);
extern u8   dss_rename_raw(const char *o, const char *n);
extern i8   dss_ffirst_raw(const char *pat, void *res, u8 attr);
extern i8   dss_fnext_raw(void *res);
extern i8   dss_getenv_raw(const char *name, char *buf);
extern i8   dss_setenv_raw(const char *nv);
extern i8   dss_expath_raw(const char *path, char *buf, u8 sub);
extern i16  dss_exec_raw(const char *cmd);
extern i16  dss_exec_ex_raw(const char *cmd);

#define IN_WIN0(p) (((u16)(p)) < 0x4000)
#define STAGE_SZ   256

static char w_stage[STAGE_SZ];      /* output / secondary buffer (WIN2) */
static char w_name[STAGE_SZ];       /* primary input string     (WIN2) */

static void cstage(char *dst, const char *src) {
    u16 i = 0;
    while (i < STAGE_SZ - 1 && src[i]) { dst[i] = src[i]; i++; }
    dst[i] = 0;
}

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
    cstage(w_name, path);
    return dss_open_raw(w_name, mode);
}

i16 dss_creat(const char *path) {
    if (!IN_WIN0(path)) return dss_creat_raw(path);
    cstage(w_name, path);
    return dss_creat_raw(w_name);
}

i16 dss_read(u8 fd, void *buf, u16 count) {
    u16 total, chunk;
    i16 n;
    u8 *p;
    if (!IN_WIN0(buf)) return dss_read_raw(fd, buf, count);
    p = (u8 *)buf; total = 0;
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
    p = (const u8 *)buf; total = 0;
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
    cstage(buf, w_stage);
    return r;
}

u8 dss_delete(const char *path) {
    if (!IN_WIN0(path)) return dss_delete_raw(path);
    cstage(w_name, path);
    return dss_delete_raw(w_name);
}

u8 dss_chdir(const char *path) {
    if (!IN_WIN0(path)) return dss_chdir_raw(path);
    cstage(w_name, path);
    return dss_chdir_raw(w_name);
}

u8 dss_mkdir(const char *path) {
    if (!IN_WIN0(path)) return dss_mkdir_raw(path);
    cstage(w_name, path);
    return dss_mkdir_raw(w_name);
}

u8 dss_rmdir(const char *path) {
    if (!IN_WIN0(path)) return dss_rmdir_raw(path);
    cstage(w_name, path);
    return dss_rmdir_raw(w_name);
}

u8 dss_rename(const char *o, const char *n) {
    if (IN_WIN0(o)) { cstage(w_name, o); o = w_name; }
    if (IN_WIN0(n)) { cstage(w_stage, n); n = w_stage; }
    return dss_rename_raw(o, n);
}

i8 dss_ffirst(const char *pat, dss_find_t *res, u8 attr) {
    i8 r;
    if (IN_WIN0(pat)) { cstage(w_name, pat); pat = w_name; }
    if (!IN_WIN0(res)) return dss_ffirst_raw(pat, res, attr);
    r = dss_ffirst_raw(pat, w_stage, attr);
    memcpy(res, w_stage, sizeof(dss_find_t));
    return r;
}

i8 dss_fnext(dss_find_t *res) {
    i8 r;
    if (!IN_WIN0(res)) return dss_fnext_raw(res);
    memcpy(w_stage, res, sizeof(dss_find_t));   /* preserve search state */
    r = dss_fnext_raw(w_stage);
    memcpy(res, w_stage, sizeof(dss_find_t));
    return r;
}

i8 dss_getenv(const char *name, char *buf) {
    i8 r;
    if (IN_WIN0(name)) { cstage(w_name, name); name = w_name; }
    if (!IN_WIN0(buf)) return dss_getenv_raw(name, buf);
    r = dss_getenv_raw(name, w_stage);
    cstage(buf, w_stage);
    return r;
}

i8 dss_setenv(const char *nv) {
    if (!IN_WIN0(nv)) return dss_setenv_raw(nv);
    cstage(w_name, nv);
    return dss_setenv_raw(w_name);
}

i8 dss_expath(const char *path, char *buf, u8 sub) {
    i8 r;
    if (IN_WIN0(path)) { cstage(w_name, path); path = w_name; }
    if (!IN_WIN0(buf)) return dss_expath_raw(path, buf, sub);
    r = dss_expath_raw(path, w_stage, sub);
    cstage(buf, w_stage);
    return r;
}

i16 dss_exec(const char *cmd) {
    if (!IN_WIN0(cmd)) return dss_exec_raw(cmd);
    cstage(w_name, cmd);
    return dss_exec_raw(w_name);
}

i16 dss_exec_ex(const char *cmd, u8 *err) {
    i16 r;
    if (IN_WIN0(cmd)) { cstage(w_name, cmd); cmd = w_name; }
    r = dss_exec_ex_raw(cmd);
    if (r < 0) { if (err) *err = (u8)(r & 0xFF); return -1; }
    if (err) *err = 0;
    return r;
}
