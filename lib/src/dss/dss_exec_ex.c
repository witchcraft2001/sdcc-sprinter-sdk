#include <sprinter/dss.h>

i16 dss_exec_ex(const char *path, u8 *err) __naked {
    (void)path; (void)err;
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
}
