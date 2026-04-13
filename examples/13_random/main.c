#include <stdio.h>
#include <stdlib.h>

void main(void) {
    int i, n;

    printf("=== stdlib demo ===\n\n");

    printf("abs(-42) = %d\n", abs(-42));
    printf("atoi(\"12345\") = %d\n", atoi("12345"));

    printf("\nRandom numbers (seed=1):\n");
    srand(1);
    for (i = 0; i < 10; i++) {
        printf("  rand() = %d\n", rand());
    }

    printf("\nRandom 0-99:\n");
    srand(42);
    for (i = 0; i < 20; i++) {
        n = rand() % 100;
        printf("%3d ", n);
        if ((i % 10) == 9) printf("\n");
    }

    printf("\n\nDone. Press any key.\n");
    getchar();
}
