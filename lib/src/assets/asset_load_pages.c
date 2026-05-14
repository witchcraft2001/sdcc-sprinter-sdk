#include <sprinter/assets.h>
#include <sprinter/bios.h>
#include <sprinter/dss.h>
#include <sprinter/ports.h>

#define ASSET_PAGE_SIZE       0x4000U
#define ASSET_PACK_FLAG_RAW   0x00
#define ASSET_PACK_FLAG_HST   0x01

static u16 asset_get_u16(const u8 *p) {
    return (u16)p[0] | ((u16)p[1] << 8);
}

static i16 asset_read_exact(u8 fd, void *buf, u16 len) {
    u8 *p;
    u16 done;
    i16 n;

    p = (u8 *)buf;
    done = 0;
    while (done < len) {
        n = dss_read(fd, p + done, len - done);
        if (n <= 0) return -1;
        done += (u16)n;
    }
    return 0;
}

static i16 asset_load_raw_open(u8 fd, u8 block, u8 page_count) {
    i16 n;
    i16 loaded;
    u8 page;

    loaded = 0;
    for (page = 0; page < page_count; page++) {
        dss_setwin_page(3, block, page);
        n = dss_read(fd, (void *)0xC000, ASSET_PAGE_SIZE);
        if (n < 0) return -1;
        if (n > 0) loaded++;
        if (n < ASSET_PAGE_SIZE) break;
    }
    return loaded;
}

static i16 asset_load_packed_open(u8 fd, const u8 *header, u8 block, u8 page_count) {
    u8 chunk_header[5];
    u8 old_win3;
    u8 chunks;
    u8 chunk;
    u8 flags;
    u16 raw_len;
    u16 data_len;
    i16 n;
    i16 loaded;

    if (header[4] != ASSET_PACK_VERSION) return -1;
    chunks = header[5];
    if (chunks > page_count) return -1;

    old_win3 = inp(PORT_WIN3);
    loaded = 0;

    for (chunk = 0; chunk < chunks; chunk++) {
        if (asset_read_exact(fd, chunk_header, sizeof(chunk_header)) < 0) {
            loaded = -1;
            break;
        }

        flags = chunk_header[0];
        raw_len = asset_get_u16(chunk_header + 1);
        data_len = asset_get_u16(chunk_header + 3);
        if (raw_len == 0 || raw_len > ASSET_PAGE_SIZE ||
            data_len == 0 || data_len > ASSET_PAGE_SIZE) {
            loaded = -1;
            break;
        }

        dss_setwin_page(3, block, chunk);
        if (flags == ASSET_PACK_FLAG_RAW) {
            if (data_len != raw_len) {
                loaded = -1;
                break;
            }
            n = dss_read(fd, (void *)0xC000, raw_len);
            if (n != (i16)raw_len) {
                loaded = -1;
                break;
            }
        } else if (flags == ASSET_PACK_FLAG_HST) {
            if (asset_read_exact(fd, (void *)0xC000, data_len) < 0) {
                loaded = -1;
                break;
            }
            hrust_depack((const void *)0xC000, (void *)0xC000);
            dss_ei();
        } else {
            loaded = -1;
            break;
        }
        loaded++;
    }

    outp(PORT_WIN3, old_win3);
    return loaded;
}

i16 asset_load_pages(const char *path, u8 block, u8 page_count) {
    u8 header[8];
    u8 old_win3;
    i16 fd;
    i16 loaded;

    fd = dss_open(path, O_RDONLY);
    if (fd < 0) return -1;

    old_win3 = inp(PORT_WIN3);
    loaded = -1;

    if (asset_read_exact((u8)fd, header, sizeof(header)) == 0 &&
        header[0] == ASSET_PACK_MAGIC_0 &&
        header[1] == ASSET_PACK_MAGIC_1 &&
        header[2] == ASSET_PACK_MAGIC_2 &&
        header[3] == ASSET_PACK_MAGIC_3) {
        loaded = asset_load_packed_open((u8)fd, header, block, page_count);
    } else {
        if (dss_seek((u8)fd, 0, SEEK_SET) == 0)
            loaded = asset_load_raw_open((u8)fd, block, page_count);
    }

    outp(PORT_WIN3, old_win3);
    dss_close((u8)fd);
    return loaded;
}
