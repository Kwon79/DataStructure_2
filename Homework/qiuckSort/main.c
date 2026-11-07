#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "quick.h"

int main(void)
{
    const int sizes[] = { 1000, 2000, 4000, 8000, 16000, 20000 };
    const int count = (int)(sizeof(sizes) / sizeof(sizes[0]));
    int k;
    int n;
    int* origin;
    double rec_ms, iter_ms;

    srand((unsigned int)time(NULL));

    for (k = 0; k < count; k++) {
        n = sizes[k];
        origin = (int*)malloc(sizeof(int) * (size_t)n);
        if (origin == NULL) {
            printf("메모리 할당 실패\n");
            return 1;
        }
        generate_data(origin, n);

        printf("Data Size : %d\n\n", n);
        printf("Quick Sort (First)    : %lld comparisons\n", count_comparisons(origin, n, PIVOT_FIRST));
        printf("Quick Sort (Random)   : %lld comparisons\n", count_comparisons(origin, n, PIVOT_RANDOM));
        printf("Quick Sort (Median-3) : %lld comparisons\n\n", count_comparisons(origin, n, PIVOT_MEDIAN3));

        free(origin);
    }

    /* 보너스: 재귀 vs 반복 (200회 평균) */
    n = 10000;
    origin = (int*)malloc(sizeof(int) * (size_t)n);
    if (origin == NULL) {
        printf("메모리 할당 실패\n");
        return 1;
    }
    generate_data(origin, n);
    measure_time(origin, n, PIVOT_MEDIAN3, 200, &rec_ms, &iter_ms);

    printf("=== Bonus ===\n");
    printf("Data Size : %d\n", n);
    printf("Pivot     : Median-of-Three\n\n");
    printf("Recursive Quick Sort : %.4f ms\n", rec_ms);
    printf("Iterative Quick Sort : %.4f ms\n", iter_ms);

    free(origin);
    return 0;
}