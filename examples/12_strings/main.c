#include <stdio.h>
#include <string.h>
#include <ctype.h>

void str_toupper(char *s) {
    while (*s) { *s = toupper(*s); s++; }
}

void main(void) {
    char buf[64];
    char a[] = "Hello";
    char b[] = "World";

    printf("=== string.h demo ===\n\n");

    printf("strlen(\"%s\") = %d\n", a, strlen(a));
    printf("strcmp(\"%s\",\"%s\") = %d\n", a, b, strcmp(a, b));

    strcpy(buf, a);
    strcat(buf, ", ");
    strcat(buf, b);
    strcat(buf, "!");
    printf("concat: %s\n", buf);

    str_toupper(buf);
    printf("upper:  %s\n", buf);

    printf("strchr(\"%s\",'O') at pos %d\n", buf, (int)(strchr(buf, 'O') - buf));

    printf("\nCharacter tests:\n");
    {
        char *test = "Hello 123!";
        char *p = test;
        while (*p) {
            printf("'%c': alpha=%d digit=%d upper=%d\n",
                   *p, isalpha(*p) ? 1 : 0, isdigit(*p) ? 1 : 0, isupper(*p) ? 1 : 0);
            p++;
        }
    }

    printf("\nDone. Press any key.\n");
    getchar();
}
