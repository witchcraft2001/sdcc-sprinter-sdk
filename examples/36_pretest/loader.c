/*
 * 36_pretest - verify DSS PRELOAD (EXE LOADER-field) semantics.
 *
 * Built as a PRELOAD .EXE (win0_preexe.py): DSS loads only this loader to
 * 0x8100, leaves the .EXE file open, and jumps. The open file handle is in the
 * PSP at IX-3; with LD_ADDR=0x8100 the PSP arg buffer is at 0x8080, so the
 * handle is at 0x807D. The file position is left right after the loader, i.e.
 * at the appended blob -- so a plain DSS.Read pulls the blob in.
 *
 * If the appended blob text prints, PRELOAD works with this header size and the
 * full multi-page packager (Phase 4b) can be built on it.
 */

#include <sprinter.h>

static char buf[40];

void main(void) {
    u8 handle = *(u8 *)0x807D;          /* EXE file handle from PSP (IX-3) */

    dss_puts("PRELOAD test: reading appended blob via DSS.Read...\r\n");
    dss_read(handle, buf, 32);
    dss_puts(buf);                       /* should be the appended blob text */
    dss_close(handle);
    dss_puts("(blob text above => PRELOAD works; garbage => header size wrong)\r\n");

    dss_waitkey();
}
