/**
 * dss.c — DSS OS system call wrappers for SDCC sdcccall(1)
 *
 * sdcccall(1) rules:
 *   Params: 1st u8→A, 1st u16/ptr→HL; 2nd u8→L(if 1st=A), 2nd u16/ptr→DE; 3rd+→stack
 *   Return: u8→A, u16/ptr→DE
 *   Callee-saved: IX only
 *   CALLEE must clean stack params before ret!
 *
 * DSS (RST #10) clobbers IX — push/pop IX around all calls.
 *
 * Stack cleanup: pop return addr into IY, adjust SP, jp (iy).
 *
 * DSS open modes (real DSS source): A: 0=R/W, 1=Read, 2=Write
 */

#include <sprinter/dss.h>

extern u16 _cmdline;

/* ===== Process ===== */

void dss_exit(u8 code) __naked {
    __asm
        ld      b, a
        ld      c, #0x41
        rst     #0x10
        ret
    __endasm;
}

/* ===== Console Output ===== */

void dss_putchar(u8 ch) __naked {
    __asm
        push    ix
        cp      #0x0A
        jr      nz, _dss_pc_out
        push    af
        ld      a, #0x0D
        ld      c, #0x5B
        rst     #0x10
        pop     af
_dss_pc_out:
        ld      c, #0x5B
        rst     #0x10
        pop     ix
        ret
    __endasm;
}

void dss_puts(const char *str) __naked {
    __asm
        push    ix
        ld      c, #0x5C
        rst     #0x10
        pop     ix
        ret
    __endasm;
}

/* ===== Keyboard ===== */

u8 dss_waitkey(void) __naked {
    __asm
        push    ix
        ld      c, #0x30
        rst     #0x10
        pop     ix
        ret
    __endasm;
}

bool dss_kbhit(void) __naked {
    __asm
        push    ix
        ld      c, #0x33
        rst     #0x10
        pop     ix
        or      a
        ret     z
        ld      a, #1
        ret
    __endasm;
}

u16 dss_getche(void) __naked {
    __asm
        push    ix
        ld      c, #0x32
        rst     #0x10
        pop     ix
        ret
    __endasm;
}

/* ===== Cursor / Screen ===== */

void dss_gotoxy(u8 x, u8 y) __naked {
    /* x in A, y in L — no stack params */
    __asm
        ld      e, a
        ld      d, l
        dec     d
        dec     e
        push    ix
        ld      c, #0x52
        rst     #0x10
        pop     ix
        ret
    __endasm;
}

void dss_clrscr(void) __naked {
    __asm
        push    ix
        ld      de, #0x0000
        ld      hl, #0x2050
        ld      bc, #0x0756
        ld      a, #0x20
        rst     #0x10
        ld      de, #0x0000
        ld      c, #0x52
        rst     #0x10
        pop     ix
        ret
    __endasm;
}

/* ===== Disk ===== */

u8 dss_getdisk(void) __naked {
    __asm
        push    ix
        ld      c, #0x02
        rst     #0x10
        pop     ix
        ret
    __endasm;
}

void dss_setdisk(u8 disk) __naked {
    __asm
        push    ix
        ld      c, #0x01
        rst     #0x10
        pop     ix
        ret
    __endasm;
}

/* ===== File I/O ===== */

i16 dss_open(const char *path, u8 mode) __naked {
    /*  path in HL, mode on stack (1 byte). Callee must clean 1 byte.
     *  DSS: A=mode (0=R/W, 1=Read, 2=Write), HL=path, C=0x11 */
    (void)path; (void)mode;
    __asm
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      a, 0 (iy)      ; mode from stack
        inc     a               ; O_RDONLY(0)→1, O_WRONLY(1)→2, O_RDWR(2)→3
        cp      #3
        jr      nz, _dss_open_go
        xor     a               ; O_RDWR: 3→0 (DSS R/W mode)
_dss_open_go:
        ld      c, #0x11
        rst     #0x10
        pop     ix
        jr      c, _dss_open_err
        ld      e, a
        ld      d, #0
        ; clean 1 byte of stack param, return DE
        pop     iy
        inc     sp
        jp      (iy)
_dss_open_err:
        ld      de, #0xFFFF
        pop     iy
        inc     sp
        jp      (iy)
    __endasm;
}

i16 dss_creat(const char *path) __naked {
    /* path in HL. No stack params. */
    __asm
        push    ix
        ld      a, #0x20        ; attribute: archive
        ld      c, #0x0A
        rst     #0x10
        pop     ix
        jr      c, _dss_creat_err
        ld      e, a
        ld      d, #0
        ret
_dss_creat_err:
        ld      de, #0xFFFF
        ret
    __endasm;
}

u8 dss_close(u8 fd) __naked {
    /* fd in A. No stack params. */
    __asm
        push    ix
        ld      c, #0x12
        rst     #0x10
        pop     ix
        ret
    __endasm;
}

i16 dss_read(u8 fd, void *buf, u16 count) __naked {
    /* fd in A, buf in DE, count on stack (2 bytes). Callee cleans 2 bytes. */
    (void)fd; (void)buf; (void)count;
    __asm
        push    ix
        ex      de, hl          ; HL = buf
        ld      iy, #4
        add     iy, sp
        ld      e, 0 (iy)
        ld      d, 1 (iy)      ; DE = count
        ld      c, #0x13
        rst     #0x10
        pop     ix
        jr      c, _dss_read_err
        ; DE = bytes read (set by DSS)
        pop     iy              ; return address
        inc     sp
        inc     sp              ; clean 2 bytes
        jp      (iy)
_dss_read_err:
        ld      de, #0xFFFF
        pop     iy
        inc     sp
        inc     sp
        jp      (iy)
    __endasm;
}

i16 dss_write(u8 fd, const void *buf, u16 count) __naked {
    /* fd in A, buf in DE, count on stack (2 bytes). Callee cleans 2 bytes.
     * Note: emulator bug — doesn't set DE after write. We save count
     * and return it as bytes written on success. */
    (void)fd; (void)buf; (void)count;
    __asm
        push    ix
        ex      de, hl          ; HL = buf
        ld      iy, #4
        add     iy, sp
        ld      e, 0 (iy)
        ld      d, 1 (iy)      ; DE = count
        push    de              ; save count (emulator doesn't return DE)
        ld      c, #0x14
        rst     #0x10
        pop     de              ; DE = count (workaround for emulator bug)
        pop     ix
        jr      c, _dss_write_err
        ; return DE = count written
        pop     iy
        inc     sp
        inc     sp
        jp      (iy)
_dss_write_err:
        ld      de, #0xFFFF
        pop     iy
        inc     sp
        inc     sp
        jp      (iy)
    __endasm;
}

i16 dss_seek(u8 fd, u32 offset, u8 origin) __naked {
    /* fd in A, offset(4) + origin(1) on stack = 5 bytes. Callee cleans 5 bytes. */
    (void)fd; (void)offset; (void)origin;
    __asm
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)      ; HL = offset low word
        push    hl
        pop     ix              ; IX = offset low word
        ld      l, 2 (iy)
        ld      h, 3 (iy)      ; HL = offset high word
        ld      b, 4 (iy)      ; origin
        ld      c, #0x15
        rst     #0x10
        pop     ix
        jr      c, _dss_seek_err
        ld      de, #0
        pop     iy
        ld      hl, #5
        add     hl, sp
        ld      sp, hl
        jp      (iy)
_dss_seek_err:
        ld      de, #0xFFFF
        pop     iy
        ld      hl, #5
        add     hl, sp
        ld      sp, hl
        jp      (iy)
    __endasm;
}

u8 dss_delete(const char *path) __naked {
    /* path in HL. No stack params. */
    __asm
        push    ix
        ld      c, #0x0E
        rst     #0x10
        pop     ix
        jr      c, _dss_del_err
        xor     a
        ret
_dss_del_err:
        ret
    __endasm;
}

u8 dss_rename(const char *oldpath, const char *newpath) __naked {
    /* oldpath in HL, newpath in DE. No stack params. */
    __asm
        push    ix
        ld      c, #0x10
        rst     #0x10
        pop     ix
        jr      c, _dss_ren_err
        xor     a
        ret
_dss_ren_err:
        ret
    __endasm;
}

u8 dss_chdir(const char *path) __naked {
    /* path in HL. No stack params. */
    __asm
        push    ix
        ld      c, #0x1D
        rst     #0x10
        pop     ix
        jr      c, _dss_cd_err
        xor     a
        ret
_dss_cd_err:
        ret
    __endasm;
}

/* ===== Directory Search ===== */

i8 dss_ffirst(const char *pattern, dss_find_t *result, u8 attr) __naked {
    /* pattern in HL, result in DE, attr on stack (1 byte). Callee cleans 1 byte. */
    (void)pattern; (void)result; (void)attr;
    __asm
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      a, 0 (iy)      ; attr
        ld      b, #1
        ld      c, #0x19
        rst     #0x10
        pop     ix
        jr      c, _dss_ff_err
        xor     a
        pop     iy
        inc     sp
        jp      (iy)
_dss_ff_err:
        ld      a, #0xFF
        pop     iy
        inc     sp
        jp      (iy)
    __endasm;
}

i8 dss_fnext(dss_find_t *result) __naked {
    /* result in HL. No stack params. */
    __asm
        push    ix
        ex      de, hl
        ld      c, #0x1A
        rst     #0x10
        pop     ix
        jr      c, _dss_fn_err
        xor     a
        ret
_dss_fn_err:
        ld      a, #0xFF
        ret
    __endasm;
}

/* ===== Date / Time ===== */

void dss_getdate(dss_date_t *d) __naked {
    __asm
        push    ix
        push    hl
        ld      c, #0x21
        rst     #0x10
        push    ix
        pop     bc
        pop     hl
        ld      (hl), c
        inc     hl
        ld      (hl), b
        inc     hl
        ld      (hl), d
        inc     hl
        ld      (hl), e
        pop     ix
        ret
    __endasm;
}

void dss_gettime(dss_time_t *t) __naked {
    __asm
        push    ix
        push    hl
        ld      c, #0x21
        rst     #0x10
        ex      de, hl
        pop     hl
        ld      (hl), e
        inc     hl
        ld      (hl), d
        inc     hl
        ld      (hl), #0
        inc     hl
        ld      (hl), b
        pop     ix
        ret
    __endasm;
}

/* ===== Interrupts ===== */

void dss_ei(void) __naked {
    __asm
        ei
        ret
    __endasm;
}

void dss_di(void) __naked {
    __asm
        di
        ret
    __endasm;
}

/* ===== Memory Management ===== */

void dss_setwin(u8 win, u8 page) __naked {
    /* win in A, page in L. No stack params. */
    __asm
        push    ix
        ld      b, l
        ld      c, #0x38
        rst     #0x10
        pop     ix
        ret
    __endasm;
}

u8 dss_getmem(void) __naked {
    __asm
        push    ix
        ld      c, #0x3D
        rst     #0x10
        pop     ix
        jr      c, _dss_gm_err
        ret
_dss_gm_err:
        ld      a, #0xFF
        ret
    __endasm;
}

void dss_freemem(u8 page) __naked {
    __asm
        push    ix
        ld      c, #0x3E
        rst     #0x10
        pop     ix
        ret
    __endasm;
}

/* ===== Program Execution ===== */

i16 dss_exec(const char *path) __naked {
    __asm
        push    ix
        ld      c, #0x40
        rst     #0x10
        pop     ix
        jr      c, _dss_exec_err
        ld      e, a
        ld      d, #0
        ret
_dss_exec_err:
        ld      de, #0xFFFF
        ret
    __endasm;
}

char *dss_cmdline(void) {
    return (char *)_cmdline;
}
