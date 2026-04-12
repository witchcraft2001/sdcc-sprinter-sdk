/**
 * dss.c — DSS OS system call wrappers for SDCC
 *
 * All functions use inline Z80 assembly to call RST #10.
 * SDCC calling convention: params on stack (right to left),
 * return in A (u8), HL (u16), or DEHL (u32).
 * __z88dk_fastcall: single param in HL (or L for u8).
 */

#include <sprinter/dss.h>

/* Defined in crt0.s */
extern u16 _cmdline;

/* ===== Process ===== */

void dss_exit(u8 code) __z88dk_fastcall __naked {
    __asm
        ld      b, l            ; error code
        ld      c, #0x41        ; DSS.Exit
        rst     #0x10
        ret                     ; (never reached)
    __endasm;
}

/* ===== Console Output ===== */

void dss_putchar(u8 ch) __z88dk_fastcall __naked {
    __asm
        ld      a, l
        cp      #0x0A           ; LF?
        jr      nz, _dss_putchar_out
        ; Output CR before LF
        ld      a, #0x0D
        ld      c, #0x5B
        rst     #0x10
        ld      a, #0x0A
_dss_putchar_out:
        ld      c, #0x5B        ; DSS.PutChar
        rst     #0x10
        ret
    __endasm;
}

void dss_puts(const char *str) __z88dk_fastcall __naked {
    __asm
        ld      c, #0x5C        ; DSS.PutStr
        rst     #0x10
        ret
    __endasm;
}

/* ===== Keyboard ===== */

u8 dss_waitkey(void) __naked {
    __asm
        ld      c, #0x30        ; DSS.WaitKey
        rst     #0x10
        ld      l, a
        ret
    __endasm;
}

bool dss_kbhit(void) __naked {
    __asm
        ld      c, #0x33        ; DSS.KbHit
        rst     #0x10
        or      a
        jr      z, _dss_kbhit_no
        ld      l, #1
        ret
_dss_kbhit_no:
        ld      l, #0
        ret
    __endasm;
}

u16 dss_getche(void) __naked {
    __asm
        ld      c, #0x32        ; DSS.GetChE
        rst     #0x10
        ld      l, e            ; char code
        ld      h, d            ; scan/ext code
        ret
    __endasm;
}

/* ===== Cursor / Screen ===== */

void dss_gotoxy(u8 x, u8 y) __naked {
    __asm
        ld      hl, #2
        add     hl, sp
        ld      e, (hl)         ; x
        inc     hl
        inc     hl
        ld      d, (hl)         ; y
        dec     d               ; 1-based -> 0-based
        dec     e
        ld      c, #0x52        ; DSS.Locate
        rst     #0x10
        ret
    __endasm;
}

void dss_clrscr(void) __naked {
    __asm
        ld      de, #0x0000     ; top-left corner
        ld      hl, #0x2050     ; 32 rows, 80 cols
        ld      bc, #0x0756     ; b=attr(7=white on black), c=0x56 Clear
        ld      a, #0x20        ; fill with space
        rst     #0x10
        ; Home cursor
        ld      de, #0x0000
        ld      c, #0x52        ; DSS.Locate
        rst     #0x10
        ret
    __endasm;
}

/* ===== Disk ===== */

u8 dss_getdisk(void) __naked {
    __asm
        ld      c, #0x02        ; DSS.GetDisk
        rst     #0x10
        ld      l, a
        ret
    __endasm;
}

void dss_setdisk(u8 disk) __z88dk_fastcall __naked {
    __asm
        ld      a, l
        ld      c, #0x01        ; DSS.SetDisk (enable/select)
        rst     #0x10
        ret
    __endasm;
}

/* ===== File I/O ===== */

i16 dss_open(const char *path, u8 mode) __naked {
    __asm
        ld      hl, #2
        add     hl, sp
        ld      e, (hl)         ; path low
        inc     hl
        ld      d, (hl)         ; path high
        inc     hl
        ld      b, (hl)         ; mode
        ex      de, hl          ; HL = path
        ld      c, #0x11        ; DSS.Open
        rst     #0x10
        jr      c, _dss_open_err
        ld      l, a            ; fd in A
        ld      h, #0
        ret
_dss_open_err:
        ld      hl, #-1
        ret
    __endasm;
}

i16 dss_creat(const char *path) __naked {
    __asm
        ld      hl, #2
        add     hl, sp
        ld      e, (hl)
        inc     hl
        ld      d, (hl)
        ex      de, hl          ; HL = path
        ld      c, #0x0A        ; DSS.Creat
        rst     #0x10
        jr      c, _dss_creat_err
        ld      l, a            ; fd
        ld      h, #0
        ret
_dss_creat_err:
        ld      hl, #-1
        ret
    __endasm;
}

u8 dss_close(u8 fd) __z88dk_fastcall __naked {
    __asm
        ld      a, l            ; fd
        ld      c, #0x12        ; DSS.Close
        rst     #0x10
        ld      l, a
        ret
    __endasm;
}

i16 dss_read(u8 fd, void *buf, u16 count) __naked {
    __asm
        ld      hl, #2
        add     hl, sp
        ld      a, (hl)         ; fd
        inc     hl
        inc     hl
        ld      e, (hl)         ; buf low
        inc     hl
        ld      d, (hl)         ; buf high
        inc     hl
        ld      c, (hl)         ; count low -> will be in HL
        inc     hl
        ld      b, (hl)         ; count high
        push    bc
        pop     hl              ; HL = count
        ex      de, hl          ; DE = count, HL = buf
        ld      c, #0x13        ; DSS.Read
        push    ix
        rst     #0x10
        pop     ix
        jr      c, _dss_read_err
        ; DE = bytes actually read
        ex      de, hl
        ret
_dss_read_err:
        ld      hl, #-1
        ret
    __endasm;
}

i16 dss_write(u8 fd, const void *buf, u16 count) __naked {
    __asm
        ld      hl, #2
        add     hl, sp
        ld      a, (hl)         ; fd
        inc     hl
        inc     hl
        ld      e, (hl)         ; buf low
        inc     hl
        ld      d, (hl)         ; buf high
        inc     hl
        ld      c, (hl)         ; count low
        inc     hl
        ld      b, (hl)         ; count high
        push    bc
        pop     hl              ; HL = count
        ex      de, hl          ; DE = count, HL = buf
        ld      c, #0x14        ; DSS.Write
        push    ix
        rst     #0x10
        pop     ix
        jr      c, _dss_write_err
        ex      de, hl          ; bytes written
        ret
_dss_write_err:
        ld      hl, #-1
        ret
    __endasm;
}

i16 dss_seek(u8 fd, u32 offset, u8 origin) __naked {
    __asm
        ld      hl, #2
        add     hl, sp
        ld      a, (hl)         ; fd
        inc     hl
        inc     hl
        ld      e, (hl)         ; offset byte 0 (low)
        inc     hl
        ld      d, (hl)         ; offset byte 1
        inc     hl
        push    de              ; save low word
        ld      e, (hl)         ; offset byte 2
        inc     hl
        ld      d, (hl)         ; offset byte 3 (high)
        inc     hl
        ld      b, (hl)         ; origin
        ; DE = high word, stack has low word
        pop     hl              ; HL = low word of offset
        ld      c, #0x15        ; DSS.Seek
        push    ix
        rst     #0x10
        pop     ix
        jr      c, _dss_seek_err
        ld      hl, #0
        ret
_dss_seek_err:
        ld      hl, #-1
        ret
    __endasm;
}

u8 dss_delete(const char *path) __z88dk_fastcall __naked {
    __asm
        ld      c, #0x0B        ; DSS.Delete
        rst     #0x10
        jr      c, _dss_del_err
        ld      l, #0
        ret
_dss_del_err:
        ld      l, a
        ret
    __endasm;
}

u8 dss_rename(const char *oldpath, const char *newpath) __naked {
    __asm
        ld      hl, #2
        add     hl, sp
        ld      e, (hl)         ; oldpath low
        inc     hl
        ld      d, (hl)         ; oldpath high
        inc     hl
        ld      a, (hl)         ; newpath low -> will go to DE
        inc     hl
        ld      b, (hl)         ; newpath high
        ex      de, hl          ; HL = oldpath
        ld      d, b
        ld      e, a            ; DE = newpath
        ld      c, #0x0C        ; DSS.Rename
        rst     #0x10
        jr      c, _dss_ren_err
        ld      l, #0
        ret
_dss_ren_err:
        ld      l, a
        ret
    __endasm;
}

u8 dss_chdir(const char *path) __z88dk_fastcall __naked {
    __asm
        ld      c, #0x0E        ; DSS.ChDir
        rst     #0x10
        jr      c, _dss_cd_err
        ld      l, #0
        ret
_dss_cd_err:
        ld      l, a
        ret
    __endasm;
}

/* ===== Directory Search ===== */

i8 dss_ffirst(const char *pattern, dss_find_t *result, u8 attr) __naked {
    __asm
        ld      hl, #2
        add     hl, sp
        ld      e, (hl)         ; pattern low
        inc     hl
        ld      d, (hl)         ; pattern high
        inc     hl
        push    de              ; save pattern
        ld      e, (hl)         ; result low
        inc     hl
        ld      d, (hl)         ; result high
        inc     hl
        ld      a, (hl)         ; attr
        pop     hl              ; HL = pattern
        ld      b, #1           ; format "filename.ext"
        ld      c, #0x19        ; DSS.FFirst
        push    ix
        rst     #0x10
        pop     ix
        jr      c, _dss_ff_err
        ld      l, #0
        ret
_dss_ff_err:
        ld      l, #-1
        ret
    __endasm;
}

i8 dss_fnext(dss_find_t *result) __z88dk_fastcall __naked {
    __asm
        ex      de, hl          ; DE = result buffer
        ld      c, #0x1A        ; DSS.FNext
        push    ix
        rst     #0x10
        pop     ix
        jr      c, _dss_fn_err
        ld      l, #0
        ret
_dss_fn_err:
        ld      l, #-1
        ret
    __endasm;
}

/* ===== Date / Time ===== */

void dss_getdate(dss_date_t *d) __z88dk_fastcall __naked {
    __asm
        push    hl              ; save pointer
        ld      c, #0x21        ; DSS.SysTime
        push    ix
        rst     #0x10
        pop     ix
        ; IX = year, D = day, E = month
        pop     hl              ; restore pointer
        push    ix
        pop     bc              ; BC = year
        ld      (hl), c         ; year low
        inc     hl
        ld      (hl), b         ; year high
        inc     hl
        ld      (hl), d         ; day
        inc     hl
        ld      (hl), e         ; month
        ret
    __endasm;
}

void dss_gettime(dss_time_t *t) __z88dk_fastcall __naked {
    __asm
        push    hl              ; save pointer
        ld      c, #0x21        ; DSS.SysTime
        push    ix
        rst     #0x10
        pop     ix
        ; HL = hours:minutes, B = seconds
        ex      de, hl          ; DE = hours:minutes
        pop     hl              ; restore pointer
        ld      (hl), e         ; minute
        inc     hl
        ld      (hl), d         ; hour
        inc     hl
        ld      (hl), #0        ; hundredths (unused)
        inc     hl
        ld      (hl), b         ; seconds
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
    __asm
        ld      hl, #2
        add     hl, sp
        ld      a, (hl)         ; win
        inc     hl
        inc     hl
        ld      b, (hl)         ; page
        ld      c, #0x38        ; DSS.SetWin
        rst     #0x10
        ret
    __endasm;
}

u8 dss_getmem(void) __naked {
    __asm
        ld      c, #0x3D        ; DSS.GetMem
        rst     #0x10
        jr      c, _dss_gm_err
        ld      l, a            ; page number
        ret
_dss_gm_err:
        ld      l, #0xFF
        ret
    __endasm;
}

void dss_freemem(u8 page) __z88dk_fastcall __naked {
    __asm
        ld      a, l            ; page number
        ld      c, #0x3E        ; DSS.FreeMem
        rst     #0x10
        ret
    __endasm;
}

/* ===== Program Execution ===== */

i16 dss_exec(const char *path) __z88dk_fastcall __naked {
    __asm
        ld      c, #0x40        ; DSS.Exec
        rst     #0x10
        jr      c, _dss_exec_err
        ld      l, a
        ld      h, #0
        ret
_dss_exec_err:
        ld      hl, #-1
        ret
    __endasm;
}

char *dss_cmdline(void) {
    return (char *)_cmdline;
}
