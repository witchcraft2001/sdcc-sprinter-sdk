#include <sprinter/dss.h>

/* Turbo C fnsplit flags */
#define WILDCARDS   0x01
#define EXTENSION   0x02
#define FILENAME    0x04
#define DIRECTORY   0x08
#define DRIVE       0x10

int fnsplit(const char *path, char *drive, char *dir,
            char *fname, char *ext)
{
    int flags = 0;

    if (drive) {
        if (dss_expath(path, drive, 1) == 0 && drive[0])
            flags |= DRIVE;
    }
    if (dir) {
        if (dss_expath(path, dir, 2) == 0 && dir[0])
            flags |= DIRECTORY;
    }
    if (fname) {
        if (dss_expath(path, fname, 3) == 0 && fname[0])
            flags |= FILENAME;
    }
    if (ext) {
        if (dss_expath(path, ext, 4) == 0 && ext[0])
            flags |= EXTENSION;
    }
    return flags;
}
