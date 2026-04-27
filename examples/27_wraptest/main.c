/**
 * 27_wraptest -- Regression checks for low-level DSS/video wrappers
 */

#include <stdio.h>
#include <string.h>
#include <sprinter.h>
#include <sprinter/video.h>
#include <sprinter/bios.h>
#include <io.h>

static int fails;

static void check(int ok, const char *name) {
    printf("%s: %s\n", name, ok ? "OK" : "FAIL");
    if (!ok) fails++;
}

static void test_file_wrappers(void) {
    static char buf[16];
    const char *msg = "WRAP";
    i16 fd;
    i16 rc;

    dss_delete("WRAP.TMP");
    dss_delete("WRAP.BAK");

    fd = dss_creat("WRAP.TMP");
    check(fd >= 0, "dss_creat");
    if (fd < 0) return;

    rc = dss_write((u8)fd, msg, 4);
    check(rc == 4, "dss_write");
    rc = dss_seek((u8)fd, 0UL, SEEK_SET);
    check(rc == 0, "dss_seek");
    rc = dss_close((u8)fd);
    check(rc == 0, "dss_close");

    fd = dss_open("WRAP.TMP", O_RDONLY);
    check(fd >= 0, "dss_open");
    if (fd < 0) return;

    memset(buf, 0, sizeof(buf));
    rc = dss_read((u8)fd, buf, 4);
    check(rc == 4 && strcmp(buf, "WRAP") == 0, "dss_read");
    rc = dss_close((u8)fd);
    check(rc == 0, "dss_close ro");

    rc = dss_rename("WRAP.TMP", "WRAP.BAK");
    check(rc == 0, "dss_rename");
    rc = dss_delete("WRAP.BAK");
    check(rc == 0, "dss_delete");
}

static void test_time_wrappers(void) {
    dss_date_t d;
    dss_time_t t;

    memset(&d, 0xAA, sizeof(d));
    memset(&t, 0xAA, sizeof(t));
    dss_getdate(&d);
    dss_gettime(&t);

    check(d.year >= 1980 && d.month >= 1 && d.month <= 12 && d.day >= 1 && d.day <= 31, "dss_getdate");
    check(t.hour < 24 && t.minute < 60 && t.second < 60 && t.hundredths == 0, "dss_gettime");
}

static void test_mem_video_wrappers(void) {
    u16 total = 0;
    u16 free_pages = 0;
    u8 old_win3;
    u8 mapped;
    u8 block;
    volatile u8 *p = (volatile u8 *)0xC000;

    dss_meminfo(&total, &free_pages);
    check(total != 0 && free_pages <= total, "dss_meminfo");

    old_win3 = inp(0x00E2);
    video_mapvram(3, 0x50);
    mapped = inp(0x00E2);
    outp(0x00E2, old_win3);
    check(mapped == 0x50, "video_mapvram");

    block = dss_getmem();
    check(block != 0xFF, "dss_getmem");
    if (block != 0xFF) {
        dss_setwin(3, block);
        *p = 0x5A;
        dss_setwin_page(3, block, 0);
        check(*p == 0x5A, "dss_setwin");
        outp(0x00E2, old_win3);
        dss_freemem(block);
    }
}

static void test_video_mode_wrapper(void) {
    u8 mode = 0xAA;
    u8 page = 0xAA;
    u8 mode2;

    dss_getvmod(&mode, &page);
    mode2 = video_getmode();
    check(mode != 0xAA && mode == mode2, "dss_getvmod");
}

void main(void) {
    fails = 0;
    printf("=== wrapper regression ===\n\n");

    test_file_wrappers();
    test_time_wrappers();
    test_mem_video_wrappers();
    test_video_mode_wrapper();

    if (fails) {
        printf("\nResult: FAIL (%d failures)\n", fails);
    } else {
        printf("\nResult: OK\n");
    }
    printf("Press any key...\n");
    getchar();
}
