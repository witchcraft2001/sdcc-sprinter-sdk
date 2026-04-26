        .module video_vsync
        .globl  _video_vsync

        .area   _CODE

_video_vsync::
        ; DSS keeps the platform interrupt handlers active. A plain HALT is
        ; safer here than polling #FE.5: on some setups that sync bit is stuck.
        ei
        halt
        ret
