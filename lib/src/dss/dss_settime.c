#include <sprinter/dss.h>

void dss_settime(dss_date_t *d, dss_time_t *t) __naked {
    (void)d; (void)t;
    __asm
        push    ix
        ; HL = dss_date_t*: year(u16), day(u8), month(u8)
        ; DE = dss_time_t*: minute(u8), hour(u8), hundredths(u8), second(u8)
        push    de              ; save time ptr
        ; Load date struct
        ld      e, (hl)
        inc     hl
        ld      d, (hl)         ; DE = year
        inc     hl
        push    de
        pop     ix              ; IX = year
        ld      d, (hl)         ; D = day
        inc     hl
        ld      e, (hl)         ; E = month
        ; Load time struct
        pop     hl              ; HL = dss_time_t*
        ld      a, (hl)         ; A = minute
        inc     hl
        push    af              ; save minute
        ld      a, (hl)         ; A = hour
        inc     hl
        inc     hl              ; skip hundredths
        ld      b, (hl)         ; B = second
        ld      h, a            ; H = hour
        pop     af
        ld      l, a            ; L = minute
        ; Registers: D=day, E=month, H=hour, L=minute, B=second, IX=year
        ld      c, #0x22
        rst     #0x10
        pop     ix
        ret
    __endasm;
}
