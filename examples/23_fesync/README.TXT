FESYNC.EXE checks Sprinter port #FE bit 5.

It reads #FFFE 32768 times with CBL off and then with CBL on.
Each pass prints:
LOW   - number of samples where bit 5 was 0
HIGH  - number of samples where bit 5 was 1
TRANS - number of 0<->1 transitions

Expected on real Sprinter or fixed emulator:
CBL off: HIGH=0000, TRANS=0000.
CBL on: LOW > 0, HIGH > 0, TRANS > 0.

Current MAME symptom for CBL off:
bit 5 is stuck high:
LOW=0000, HIGH > 0, TRANS=0000.
