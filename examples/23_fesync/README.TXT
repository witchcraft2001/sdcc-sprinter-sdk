FESYNC.EXE checks Sprinter port #FE bit 5.

It reads #FFFE 32768 times with CBL off and then with CBL on.
CBL is switched through the canonical Sprinter CBL_DIR port #004E,
the same way DOOM2 does it, not through a direct OUT (#89),A.
Each pass prints:
LOW   - number of samples where bit 5 was 0
HIGH  - number of samples where bit 5 was 1
TRANS - number of 0<->1 transitions

Expected on real Sprinter or fixed emulator:
CBL off: TRANS=0000. The fixed level may be LOW or HIGH.
CBL on: LOW > 0, HIGH > 0, TRANS > 0.

Bad emulator or setup symptom:
CBL on still has no LOW/HIGH transitions.
