        .module fesync
        .globl  _main

        .area   _CODE

_main::
        ld      hl,#msg_title
        call    puts

        xor     a               ; CBL off
        out     (#0x89),a
        ld      hl,#msg_off
        call    puts
        call    sample_fe5
        call    print_results

        ld      a,#0x80         ; CBL on, 15 kHz mono
        out     (#0x89),a
        ld      hl,#msg_on
        call    puts
        call    sample_fe5
        call    print_results

        xor     a               ; leave CBL disabled
        out     (#0x89),a

        ld      hl,#msg_expect1
        call    puts
        ld      hl,#msg_expect2
        call    puts
        ld      hl,#msg_expect3
        call    puts
        ld      hl,#msg_key
        call    puts

        ld      c,#0x30        ; DSS WaitKey
        rst     #0x10
        ret

sample_fe5:
        xor     a
        ld      (low_cnt),a
        ld      (low_cnt+1),a
        ld      (high_cnt),a
        ld      (high_cnt+1),a
        ld      (trans_cnt),a
        ld      (trans_cnt+1),a
        ld      a,#0xff
        ld      (prev_bit),a

        ld      bc,#0x8000
sample_loop:
        ld      a,#0xff        ; high address byte: read #FFFE, no ZX key row selected
        in      a,(#0xfe)
        and     #0x20
        jr      z,saw_low

saw_high:
        ld      hl,#high_cnt
        call    inc16_at_hl
        ld      e,#1
        jr      got_bit

saw_low:
        ld      hl,#low_cnt
        call    inc16_at_hl
        ld      e,#0

got_bit:
        ld      a,(prev_bit)
        cp      #0xff
        jr      z,set_prev
        cp      e
        jr      z,set_prev
        ld      hl,#trans_cnt
        call    inc16_at_hl

set_prev:
        ld      a,e
        ld      (prev_bit),a
        dec     bc
        ld      a,b
        or      c
        jr      nz,sample_loop
        ret

print_results:
        ld      hl,#msg_low
        call    puts
        ld      hl,#low_cnt
        call    print16_at_hl
        call    crlf

        ld      hl,#msg_high
        call    puts
        ld      hl,#high_cnt
        call    print16_at_hl
        call    crlf

        ld      hl,#msg_trans
        call    puts
        ld      hl,#trans_cnt
        call    print16_at_hl
        call    crlf
        call    crlf
        ret

puts:
        ld      c,#0x5c        ; DSS PrintString, HL=zero-terminated string
        rst     #0x10
        ret

putc:
        ld      c,#0x5b        ; DSS PutChar, A=character
        rst     #0x10
        ret

crlf:
        ld      a,#0x0d
        call    putc
        ld      a,#0x0a
        jp      putc

inc16_at_hl:
        inc     (hl)
        ret     nz
        inc     hl
        inc     (hl)
        ret

print16_at_hl:
        inc     hl
        ld      a,(hl)
        call    print_hex8
        dec     hl
        ld      a,(hl)
        jp      print_hex8

print_hex8:
        push    af
        rrca
        rrca
        rrca
        rrca
        call    print_nibble
        pop     af

print_nibble:
        and     #0x0f
        add     a,#0x30
        cp      #0x3a
        jr      c,hex_out
        add     a,#7
hex_out:
        jp      putc

msg_title:
        .ascii  "FE sync bit test (#FFFE bit5)"
        .db     13,10,0
msg_off:
        .ascii  "CBL off, sampling 32768 reads..."
        .db     13,10,0
msg_on:
        .ascii  "CBL on (#89=#80), sampling 32768 reads..."
        .db     13,10,0
msg_low:
        .ascii  "LOW     = "
        .db     0
msg_high:
        .ascii  "HIGH    = "
        .db     0
msg_trans:
        .ascii  "TRANS   = "
        .db     0
msg_expect1:
        .ascii  "Expected CBL off: HIGH=0000 TRANS=0000"
        .db     13,10,0
msg_expect2:
        .ascii  "Expected CBL on : LOW>0 HIGH>0 TRANS>0"
        .db     13,10,0
msg_expect3:
        .ascii  "MAME bug CBL off: LOW=0000, bit stuck high"
        .db     13,10,13,10,0
msg_key:
        .ascii  "Press any key to exit..."
        .db     13,10,0

low_cnt:
        .ds     2
high_cnt:
        .ds     2
trans_cnt:
        .ds     2
prev_bit:
        .ds     1
