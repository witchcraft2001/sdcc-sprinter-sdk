#include <sprinter/dss.h>

i16 dss_exec_ex(const char *path, u8 *err) __naked {
    (void)path; (void)err;
#if defined(SDCC) && (SDCC < 300)
    __asm
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      e, 2 (iy)
        ld      d, 3 (iy)
        push    de
        ld      b, #0
        ld      c, #0x40
        rst     #0x10
        pop     de
        pop     ix
        jr      c, _dss_exec_ex_err_290
        ld      b, a
        ld      a, d
        or      e
        jr      z, _dss_exec_ex_ok_ret_290
        xor     a
        ld      (de), a
_dss_exec_ex_ok_ret_290:
        ld      l, b
        ld      h, #0
        ret
_dss_exec_ex_err_290:
        ld      b, a
        ld      a, d
        or      e
        jr      z, _dss_exec_ex_set_ret_290
        ld      a, b
        ld      (de), a
_dss_exec_ex_set_ret_290:
        ld      hl, #0xFFFF
        ret
    __endasm;
#else
    __asm
        ; sdcccall(1): path -> HL, err -> DE
        push    ix
        push    de
        ld      b, #0           ; subfunction: EXEC0 (auto path handling)
        ld      c, #0x40
        rst     #0x10
        pop     hl              ; HL = err pointer
        jr      c, _dss_exec_ex_err

        ; success: A = child exit code
        ld      d, #0
        ld      e, a
        ld      a, h
        or      l
        jr      z, _dss_exec_ex_ok_ret
        xor     a
        ld      (hl), a
_dss_exec_ex_ok_ret:
        pop     ix
        ret

_dss_exec_ex_err:
        ld      e, a            ; save DSS error code
        ld      a, h
        or      l
        jr      z, _dss_exec_ex_set_ret
        ld      a, e
        ld      (hl), a
_dss_exec_ex_set_ret:
        ld      de, #0xFFFF
        pop     ix
        ret
    __endasm;
#endif
}
