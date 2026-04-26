        .module gfx_blit
        .globl  _gfx_blit_rect

        .area   _CODE

; A = screen, HL = x. Returns HL = 0xC000 + x + screen * 320.
gfx_blit_dest_addr:
        push    de
        ld      de, #0xC000
        add     hl, de
        and     #1
        jr      z, 1$
        ld      de, #0x0140
        add     hl, de
1$:
        pop     de
        ret

_gfx_blit_rect::
        push    ix
        ld      iy, #4
        add     iy, sp

        ld      l, 1 (iy)
        ld      h, 2 (iy)
        ld      a, 0 (iy)
        call    gfx_blit_dest_addr
        ld      (_gfx_blit_dst_addr), hl

        ld      l, 5 (iy)
        ld      h, 6 (iy)
        ld      a, 4 (iy)
        call    gfx_blit_dest_addr
        ld      (_gfx_blit_src_addr), hl

        ld      a, 3 (iy)
        ld      (_gfx_blit_dst_y), a
        ld      a, 7 (iy)
        ld      (_gfx_blit_src_y), a
        ld      a, 8 (iy)
        ld      (_gfx_blit_width), a
        ld      a, 9 (iy)
        ld      (_gfx_blit_count), a

        ld      a, #1
        ld      (_gfx_blit_step), a
        ld      a, 0 (iy)
        xor     4 (iy)
        and     #1
        jr      nz, blit_rect_setup_done
        ld      a, 3 (iy)
        cp      7 (iy)
        jr      z, blit_rect_setup_done
        jr      c, blit_rect_setup_done

        ld      a, #0xff
        ld      (_gfx_blit_step), a
        ld      a, 9 (iy)
        dec     a
        ld      b, a
        ld      a, (_gfx_blit_dst_y)
        add     a, b
        ld      (_gfx_blit_dst_y), a
        ld      a, (_gfx_blit_src_y)
        add     a, b
        ld      (_gfx_blit_src_y), a

blit_rect_setup_done:
        in      a, (#0xE2)
        push    af
        ld      a, #0x50
        out     (#0xE2), a

blit_rect_loop:
        ld      a, (_gfx_blit_src_y)
        out     (#0x89), a
        ld      hl, (_gfx_blit_src_addr)
        ld      de, #_gfx_blit_tmp
        di
        ld      d, d
        ld      a, (_gfx_blit_width)
        ld      l, l
        ld      a, (hl)
        ld      (de), a
        ld      b, b
        ei

        ld      a, (_gfx_blit_dst_y)
        out     (#0x89), a
        ld      hl, #_gfx_blit_tmp
        ld      de, (_gfx_blit_dst_addr)
        di
        ld      d, d
        ld      a, (_gfx_blit_width)
        ld      l, l
        ld      a, (hl)
        ld      (de), a
        ld      b, b
        ei

        ld      a, (_gfx_blit_step)
        ld      b, a
        ld      a, (_gfx_blit_src_y)
        add     a, b
        ld      (_gfx_blit_src_y), a
        ld      a, (_gfx_blit_dst_y)
        add     a, b
        ld      (_gfx_blit_dst_y), a

        ld      hl, #_gfx_blit_count
        dec     (hl)
        jr      nz, blit_rect_loop

        pop     af
        out     (#0xE2), a
        ld      a, #0xC0
        out     (#0x89), a
        pop     ix
        ret

        .area   _DATA

_gfx_blit_dst_addr:
        .ds     2
_gfx_blit_src_addr:
        .ds     2
_gfx_blit_dst_y:
        .ds     1
_gfx_blit_src_y:
        .ds     1
_gfx_blit_width:
        .ds     1
_gfx_blit_count:
        .ds     1
_gfx_blit_step:
        .ds     1
_gfx_blit_tmp:
        .ds     256
