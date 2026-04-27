#include <stdio.h>
#include <string.h>
#include <sprinter/dss.h>

#define ARG_MAX_LEN 64

/* Skip leading spaces, return pointer to next token.
   Writes '\0' at token end, advances *pp past it. */
static char *next_arg(char **pp) {
    char *p = *pp;
    char *start;
    while (*p == ' ') p++;
    if (*p == '\0' || *p == '\r' || *p == '\n') return (char *)0;
    start = p;
    while (*p && *p != ' ' && *p != '\r' && *p != '\n') p++;
    if (*p) *p++ = '\0';
    *pp = p;
    return start;
}

static void copy_arg(char *dst, const char *src, u16 dst_size) {
    u16 i = 0;
    char c;

    if (!dst_size) return;
    if (!src) {
        dst[0] = '\0';
        return;
    }

    while (src[i] && (i + 1) < dst_size) {
        c = src[i];
        if (c >= 'a' && c <= 'z') c -= ('a' - 'A');
        dst[i] = c;
        i++;
    }
    dst[i] = '\0';
}

void main(void) {
    char *cmdline = dss_cmdline();
    char *infile, *outfile;
    char infile_buf[ARG_MAX_LEN];
    char outfile_buf[ARG_MAX_LEN];
    FILE *fin, *fout;
    int c, count = 0;

    /* Parse command line */
    infile  = next_arg(&cmdline);
    outfile = next_arg(&cmdline);

    if (!infile) {
        printf("BIN2C - Binary to C array\n\n");
        printf("Usage: bin2c <infile> [outfile]\n");
        printf("  If outfile omitted, output to screen.\n");
        return;
    }

    copy_arg(infile_buf, infile, sizeof(infile_buf));
    infile = infile_buf;
    if (outfile) {
        copy_arg(outfile_buf, outfile, sizeof(outfile_buf));
        outfile = outfile_buf;
    }

    fin = fopen(infile, "r");
    if (!fin) {
        printf("Error: can't open %s\n", infile);
        return;
    }

    if (outfile) {
        fout = fopen(outfile, "w");
        if (!fout) {
            printf("Error: can't create %s\n", outfile);
            fclose(fin);
            return;
        }
    } else {
        fout = stdout;
    }

    printf("%s -> %s ...", infile, outfile ? outfile : "screen");

    fprintf(fout, "unsigned char data[] = {\r\n  ");
    while ((c = fgetc(fin)) != EOF) {
        if (count > 0) fprintf(fout, ",");
        if (count > 0 && (count % 12) == 0) fprintf(fout, "\r\n  ");
        fprintf(fout, "0x%02X", c);
        count++;
    }
    fprintf(fout, "\r\n};\r\n// %d bytes\r\n", count);

    fclose(fin);
    if (outfile) fclose(fout);
    printf(" %d bytes\n", count);
}
