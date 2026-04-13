/*
 * 08_sort — Sorting algorithms demo
 * Ported from SOLID SORT2.C (Borland C 3.1 for ZX Sprinter)
 * Demonstrates Bubble, Shell, and Quick sort with statistics.
 */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#define SIZE 15

/* Statistics counters */
unsigned int iterations;
unsigned int exchanges;
unsigned int conditions;

/* Print array contents */
void print_array(int *arr, int count) {
    int i;
    cprintf("[");
    for (i = 0; i < count; i++)
        cprintf(" %3d", arr[i]);
    cputs(" ]\n");
}

/* Bubble sort */
void bubble_sort(int *arr, int count) {
    int i, j;
    for (i = 0; i < count; i++)
        for (j = i + 1; j < count; j++) {
            if (arr[i] > arr[j]) {
                int tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
                exchanges++;
                conditions++;
            }
            iterations++;
        }
}

/* Shell sort */
void shell_sort(int *arr, int count) {
    int gaps[] = {9, 5, 3, 2, 1};
    int k, i, j, gap, x;
    for (k = 0; k < 5; k++) {
        gap = gaps[k];
        for (i = gap; i < count; ++i) {
            x = arr[i];
            for (j = i - gap; x < arr[j] && j >= 0; j -= gap) {
                arr[j + gap] = arr[j];
                iterations++;
                exchanges++;
            }
            arr[j + gap] = x;
        }
    }
}

/* Quick sort */
void quick_sort(int *arr, int left, int right) {
    int x, y, i, j;
    i = left;
    j = right;
    x = arr[(left + right) / 2];
    do {
        while (arr[i] < x && i < right) { i++; iterations++; }
        while (x < arr[j] && j > left)  { j--; iterations++; }
        if (i <= j) {
            y = arr[i];
            arr[i] = arr[j];
            arr[j] = y;
            i++;
            j--;
            exchanges++;
            conditions++;
        }
    } while (i <= j);

    if (left < j)  quick_sort(arr, left, j);
    if (i < right)  quick_sort(arr, i, right);
}

void run_sort(const char *name, int *arr, int count, int method) {
    int i;

    /* Fill with random data */
    for (i = 0; i < count; i++)
        arr[i] = rand() % 100 - 50;

    iterations = exchanges = conditions = 0;

    printf("\n--- %s ---\n", name);
    printf("Before: ");
    print_array(arr, count);

    switch (method) {
        case 1: bubble_sort(arr, count); break;
        case 2: shell_sort(arr, count); break;
        case 3: quick_sort(arr, 0, count - 1); break;
    }

    printf("After:  ");
    print_array(arr, count);
    printf("  Iterations: %u  Exchanges: %u  Conditions: %u\n",
           iterations, exchanges, conditions);
}

void main(void) {
    int arr[SIZE];

    printf("=== Sorting Algorithms Demo ===\n");
    printf("Array size: %d elements\n", SIZE);

    srand(12345);

    run_sort("Bubble Sort", arr, SIZE, 1);
    run_sort("Shell Sort",  arr, SIZE, 2);
    run_sort("Quick Sort",  arr, SIZE, 3);

    printf("\nDone! Press any key.\n");
    getch();
}
