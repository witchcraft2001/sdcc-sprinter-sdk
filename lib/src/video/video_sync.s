        .module video_sync
        .globl  _video_sync_enable
        .globl  _video_sync_disable

        .area   _CODE

_video_sync_enable::
        ld      a, #0x80        ; CBL on, 15 kHz mono; enables #FFFE bit 5
        jr      set_sync_a

_video_sync_disable::
        xor     a

set_sync_a:
        ld      bc, #0x004e
        out     (c), a
        ret
