#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "merge.h"

#define TIME_REPEAT 100     /* 실행시간은 같은 조건에서 100회 측정한 평균 */

int main(void)
{
    const int sizes[] = { 1000, 2000, 4000, 8000, 16000, 20000 };
    const SortType types[] = { SORT_MERGE, SORT_QUICK_FIRST,
                               SORT_QUICK_RANDOM, SORT_QUICK_MEDIAN3 };
    const int size_count = (int)(sizeof(sizes) / sizeof(sizes[0]));
    const int type_count = (int)(sizeof(types) / sizeof(types[0]));
    int i, j;

    srand((unsigned int)time(NULL));

    for (i = 0; i < size_count; i++) {
        int n = sizes[i];
        int* origin = (int*)malloc(sizeof(int) * (size_t)n);

        if (origin == NULL) {
            printf("[Error] memory allocation failed\n");
            return 1;
        }
        generate_data(origin, n);

        printf("Data Size : %d\n\n", n);
        printf("%-16s%-15s%-12s%s\n", "Algorithm", "Comparisons", "Time(ms)", "Memory(Bytes)");
        printf("--------------------------------------------------------\n");

        for (j = 0; j < type_count; j++) {
            SortStat st = measure_sort(origin, n, types[j], TIME_REPEAT);

            if (st.comparisons < 0) {
                printf("[Error] %s failed or result is not sorted\n", sort_name(types[j]));
                continue;
            }
            printf("%-16s%-15lld%-12.4f%lld\n",
                sort_name(types[j]), st.comparisons, st.time_ms, st.memory);
        }
        printf("\n");

        free(origin);
    }
    return 0;
}