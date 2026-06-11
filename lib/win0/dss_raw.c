/*
 * dss_raw.c - raw (no-bounce) DSS wrappers for the win0 layout.
 *
 * Same RST #10 wrappers as the standard library (lib/src/dss), but under
 * *_raw names so the win0 bounce wrappers (win0_dss.c) can call them after
 * marshalling WIN0 pointers through WIN2 staging buffers. Linked only in the
 * win0 layout; the normal layout uses the library's dss_* directly with zero
 * overhead.
 *
 * ABI: SDCC 4.5.0 __sdcccall(1). Each body is a verbatim copy of the matching
 * lib/src/dss/<fn> wrapper (register-passed args, int returns in DE), with the
 * public symbol renamed to dss_*_raw and local labels made module-unique.
 *
 * Only functions that hand a pointer to DSS *during* the RST need a raw twin
 * here; functions that fill a struct in the wrapper after the RST are already
 * win0-safe and are not duplicated.
 */
#include <sprinter/dss.h>

void dss_puts_raw(const char *s) __naked {
    (void)s;
    __asm
        push    ix
        ld      c, #0x5C
        rst     #0x10
        pop     ix
        ret
    __endasm;
}

i16 dss_open_raw(const char *path, u8 mode) __naked {
    (void)path; (void)mode;
    __asm
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      a, 0 (iy)       ; mode from stack
        inc     a
        cp      #3
        jr      nz, _raw_open_go
        xor     a
_raw_open_go:
        ld      c, #0x11
        rst     #0x10
        pop     ix
        jr      c, _raw_open_err
        ld      e, a
        ld      d, #0
        pop     iy
        inc     sp
        jp      (iy)
_raw_open_err:
        ld      de, #0xFFFF
        pop     iy
        inc     sp
        jp      (iy)
    __endasm;
}

i16 dss_creat_raw(const char *path) __naked {
    (void)path;
    __asm
        push    ix
        ld      a, #0x20        ; attribute: archive
        ld      c, #0x0A
        rst     #0x10
        pop     ix
        jr      c, _raw_creat_err
        ld      e, a
        ld      d, #0
        ret
_raw_creat_err:
        ld      de, #0xFFFF
        ret
    __endasm;
}

i16 dss_read_raw(u8 fd, void *buf, u16 count) __naked {
    (void)fd; (void)buf; (void)count;
    __asm
        push    ix
        ex      de, hl          ; HL = buf
        ld      iy, #4
        add     iy, sp
        ld      e, 0 (iy)
        ld      d, 1 (iy)       ; DE = count
        ld      c, #0x13
        rst     #0x10
        pop     ix
        jr      c, _raw_read_err
        pop     iy
        inc     sp
        inc     sp
        jp      (iy)
_raw_read_err:
        ld      de, #0xFFFF
        pop     iy
        inc     sp
        inc     sp
        jp      (iy)
    __endasm;
}

i16 dss_write_raw(u8 fd, const void *buf, u16 count) __naked {
    (void)fd; (void)buf; (void)count;
    __asm
        push    ix
        ex      de, hl          ; HL = buf
        ld      iy, #4
        add     iy, sp
        ld      e, 0 (iy)
        ld      d, 1 (iy)       ; DE = count
        push    de              ; save count (emulator does not return DE)
        ld      c, #0x14
        rst     #0x10
        pop     de
        pop     ix
        jr      c, _raw_write_err
        pop     iy
        inc     sp
        inc     sp
        jp      (iy)
_raw_write_err:
        ld      de, #0xFFFF
        pop     iy
        inc     sp
        inc     sp
        jp      (iy)
    __endasm;
}

u8 dss_curdir_raw(char *buf) __naked {
    (void)buf;
    __asm
        push    ix
        ld      c, #0x1E
        rst     #0x10
        pop     ix
        jr      c, _raw_curdir_err
        xor     a
        ret
_raw_curdir_err:
        ret
    __endasm;
}

u8 dss_delete_raw(const char *path) __naked {
    (void)path;
    __asm
        push    ix
        ld      c, #0x0E
        rst     #0x10
        pop     ix
        jr      c, _raw_delete_err
        xor     a
        ret
_raw_delete_err:
        ret
    __endasm;
}

u8 dss_chdir_raw(const char *path) __naked {
    (void)path;
    __asm
        push    ix
        ld      c, #0x1D
        rst     #0x10
        pop     ix
        jr      c, _raw_chdir_err
        xor     a
        ret
_raw_chdir_err:
        ret
    __endasm;
}

u8 dss_mkdir_raw(const char *path) __naked {
    (void)path;
    __asm
        push    ix
        ld      c, #0x1B
        rst     #0x10
        pop     ix
        jr      c, _raw_mkdir_err
        xor     a
        ret
_raw_mkdir_err:
        ret
    __endasm;
}

u8 dss_rmdir_raw(const char *path) __naked {
    (void)path;
    __asm
        push    ix
        ld      c, #0x1C
        rst     #0x10
        pop     ix
        jr      c, _raw_rmdir_err
        xor     a
        ret
_raw_rmdir_err:
        ret
    __endasm;
}

u8 dss_rename_raw(const char *oldpath, const char *newpath) __naked {
    (void)oldpath; (void)newpath;
    __asm
        push    ix
        ld      c, #0x10
        rst     #0x10
        pop     ix
        jr      c, _raw_rename_err
        xor     a
        ret
_raw_rename_err:
        ret
    __endasm;
}

i8 dss_ffirst_raw(const char *pattern, void *result, u8 attr) __naked {
    (void)pattern; (void)result; (void)attr;
    __asm
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      a, 0 (iy)       ; attr
        ld      b, #1
        ld      c, #0x19
        rst     #0x10
        pop     ix
        jr      c, _raw_ffirst_err
        xor     a
        pop     iy
        inc     sp
        jp      (iy)
_raw_ffirst_err:
        ld      a, #0xFF
        pop     iy
        inc     sp
        jp      (iy)
    __endasm;
}

i8 dss_fnext_raw(void *result) __naked {
    (void)result;
    __asm
        push    ix
        ex      de, hl
        ld      c, #0x1A
        rst     #0x10
        pop     ix
        jr      c, _raw_fnext_err
        xor     a
        ret
_raw_fnext_err:
        ld      a, #0xFF
        ret
    __endasm;
}

i8 dss_getenv_raw(const char *name, char *buf) __naked {
    (void)name; (void)buf;
    __asm
        push    ix
        ld      b, #1           ; subfunction: GETENV
        ld      c, #0x46
        rst     #0x10
        pop     ix
        jr      c, _raw_getenv_err
        xor     a
        ret
_raw_getenv_err:
        ld      a, #0xFF
        ret
    __endasm;
}

i8 dss_setenv_raw(const char *namevalue) __naked {
    (void)namevalue;
    __asm
        push    ix
        ld      b, #2           ; subfunction: SETENV
        ld      c, #0x46
        rst     #0x10
        pop     ix
        jr      c, _raw_setenv_err
        xor     a
        ret
_raw_setenv_err:
        ld      a, #0xFF
        ret
    __endasm;
}

i8 dss_expath_raw(const char *path, char *buf, u8 subfunc) __naked {
    (void)path; (void)buf; (void)subfunc;
    __asm
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      b, 0 (iy)       ; B = subfunc from stack
        ld      c, #0x45
        rst     #0x10
        pop     ix
        jr      c, _raw_expath_err
        xor     a
        pop     iy
        inc     sp
        jp      (iy)
_raw_expath_err:
        ld      a, #0xFF
        pop     iy
        inc     sp
        jp      (iy)
    __endasm;
}

/* EXEC: child-exit return is normalised by the win0 RST10 trampoline. The
 * stack dance preserves IX/IY and the SDCC return address across the call. */
i16 dss_exec_raw(const char *cmd) __naked {
    (void)cmd;
    __asm
        pop     de              ; caller return address
        push    ix
        push    iy
        push    de
        ld      b, #0
        ld      c, #0x40
        rst     #0x10
        pop     de              ; caller return address
        pop     iy
        pop     ix
        push    de
        jr      c, _raw_exec_err
        ld      c, a
        ld      e, c
        ld      d, #0
        ret
_raw_exec_err:
        ld      de, #0xFFFF
        ret
    __endasm;
}

/* EXEC with raw error capture: success -> DE = exit code (high byte 0);
 * error -> DE = 0xFF00 | code (negative i16). win0_dss.c's dss_exec_ex()
 * splits this back into a return value + *err. */
i16 dss_exec_ex_raw(const char *cmd) __naked {
    (void)cmd;
    __asm
        pop     de              ; caller return address
        push    ix
        push    iy
        push    de
        ld      b, #0
        ld      c, #0x40
        rst     #0x10
        pop     de              ; caller return address
        pop     iy
        pop     ix
        push    de
        jr      c, _raw_execex_err
        ld      e, a            ; success: exit code 0..255
        ld      d, #0
        ret
_raw_execex_err:
        ld      e, a            ; error code in low byte
        ld      d, #0xFF        ; high byte marks error (negative)
        ret
    __endasm;
}
