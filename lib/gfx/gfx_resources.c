#include <sprinter/gfx.h>
#include <sprinter/dss.h>
#include <sprinter/bios.h>
#include <sprinter/ports.h>

i16 gfx_load_resource_pages(const char *path, u8 first_page, u8 page_count) {
    i16 fd;
    i16 n;
    i16 loaded;
    u8 old_page;
    u8 page;

    fd = dss_open(path, O_RDONLY);
    if (fd < 0) return -1;

    old_page = inp(PORT_WIN3);
    loaded = 0;
    page = first_page;

    while (page_count) {
        dss_setwin(3, page);
        n = dss_read((u8)fd, (void *)0xC000, 0x4000);
        if (n < 0) {
            dss_setwin(3, old_page);
            dss_close((u8)fd);
            return -1;
        }
        if (n > 0) loaded++;
        if (n < 0x4000) break;
        page++;
        page_count--;
    }

    dss_setwin(3, old_page);
    dss_close((u8)fd);
    return loaded;
}

u8 gfx_draw_resource(u8 screen, u16 x, u8 y, u8 base_page,
                     const gfx_resource_t *resources, u8 id, u8 flags) {
    const gfx_resource_t *res;
    const void *data;
    u8 old_page;
    u8 draw_flags;

    res = resources + id;
    old_page = inp(PORT_WIN2);
    dss_setwin(2, base_page + res->page_delta);

    data = (const void *)(0x8000 + res->offset);
    draw_flags = flags | res->flags;

    if (res->width == 8 && res->height == 8) {
        gfx_draw_sprite8(screen, x, y, data, draw_flags);
    } else if (res->width == 16 && res->height == 16) {
        gfx_draw_sprite16(screen, x, y, data, draw_flags);
    } else if (res->width == 24 && res->height == 24) {
        gfx_draw_sprite24(screen, x, y, data, draw_flags);
    } else {
        dss_setwin(2, old_page);
        return 1;
    }

    dss_setwin(2, old_page);
    return 0;
}
