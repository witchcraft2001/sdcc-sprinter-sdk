        .module video_vsync
        .globl  _video_vsync

        .area   _CODE

_video_vsync::
_vsync_wait:
        in      a,(#0xC9)
        bit     5,a
        jr      z,_vsync_wait
_vsync_wait2:
        in      a,(#0xC9)
        bit     5,a
        jr      nz,_vsync_wait2
        ret
