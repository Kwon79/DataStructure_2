#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sort.h"

int main(void)
{
    const int sizes[] = { 1000, 2000, 4000, 8000, 16000, 20000 };
    const int num_sizes = (int)(sizeof(sizes) / sizeof(sizes[0]));

    seed_random(12345u);   /* 고정 시드: 매 실행, 모든 환경에서 동일한 결과 */

    for (int s = 0; s < num_sizes; s++) {
        int n = sizes[s];

        int* origin = (int*)malloc(sizeof(int) * n);
        int* work = (int*)malloc(sizeof(int) * n);
        if (origin == NULL || work == NULL) {
            printf("메모리 할당 실패\n");
            free(origin);
            free(work);
            return 1;
        }

        /* 원본 무작위 데이터 생성 (측정 제외) */
        for (int i = 0; i < n; i++)
            origin[i] = next_rand();

        /* 같은 원본의 복사본으로 각 정렬 수행 */
        memcpy(work, origin, sizeof(int) * n);
        long long c_ins = insertion_sort(work, n);
        int ok_ins = is_sorted(work, n);

        memcpy(work, origin, sizeof(int) * n);
        long long c_org = shell_sort_original(work, n);
        int ok_org = is_sorted(work, n);

        memcpy(work, origin, sizeof(int) * n);
        long long c_knu = shell_sort_knuth(work, n);
        int ok_knu = is_sorted(work, n);

        printf("Data Size : %d\n\n", n);
        printf("Insertion Sort       : %lld comparisons\n", c_ins);
        printf("Shell Sort (Original): %lld comparisons\n", c_org);
        printf("Shell Sort (Knuth)   : %lld comparisons\n", c_knu);

        if (!(ok_ins && ok_org && ok_knu))
            printf("[오류] 정렬 검증 실패! (ins=%d, org=%d, knuth=%d)\n",
                ok_ins, ok_org, ok_knu);
        printf("\n");

        free(origin);
        free(work);
    }
    return 0;
}