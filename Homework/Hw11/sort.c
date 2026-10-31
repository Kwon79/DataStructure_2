#include "sort.h"

/* 데이터 값 사이의 비교 횟수 (long long: 오버플로 방지) */
static long long g_cnt = 0;

/* ---------------------------------------------------------
 * 간격(gap) 기반 삽입 정렬 한 패스.
 * gap = 1 이면 일반 삽입 정렬과 동일하다.
 * 데이터 값 비교(a[j - gap] > key)가 일어날 때마다 g_cnt를 1 증가시킨다.
 * (반복문 인덱스 검사 j >= gap 은 카운트하지 않음)
 * --------------------------------------------------------- */
static void gap_insertion_pass(int a[], int n, int gap)
{
    for (int i = gap; i < n; i++) {
        int key = a[i];
        int j = i;
        while (j >= gap) {
            g_cnt++;                    /* 데이터 비교 1회 */
            if (a[j - gap] > key) {
                a[j] = a[j - gap];
                j -= gap;
            }
            else {
                break;
            }
        }
        a[j] = key;
    }
}

/* [1] 삽입 정렬 */
long long insertion_sort(int a[], int n)
{
    g_cnt = 0;
    gap_insertion_pass(a, n, 1);
    return g_cnt;
}

/* [2] 쉘 정렬 - 기본 간격: n/2, n/4, ..., 1 */
long long shell_sort_original(int a[], int n)
{
    g_cnt = 0;
    for (int gap = n / 2; gap >= 1; gap /= 2)
        gap_insertion_pass(a, n, gap);
    return g_cnt;
}

/* [3] 쉘 정렬 - Knuth 간격: 1, 4, 13, 40, ... (h = 3h + 1) */
long long shell_sort_knuth(int a[], int n)
{
    g_cnt = 0;

    /* n보다 작은 가장 큰 Knuth 간격 찾기 */
    int h = 1;
    while (h * 3 + 1 < n)
        h = h * 3 + 1;

    /* 역순 적용: ... -> 364 -> 121 -> 40 -> 13 -> 4 -> 1  (h = (h - 1) / 3 == h / 3) */
    for (; h >= 1; h /= 3)
        gap_insertion_pass(a, n, h);
    return g_cnt;
}

/* 오름차순 정렬 여부 검증 */
int is_sorted(const int a[], int n)
{
    for (int i = 1; i < n; i++)
        if (a[i - 1] > a[i])
            return 0;
    return 1;
}

/* ---------------------------------------------------------
 * 난수 생성기 (xorshift32)
 * MSVC의 rand()는 0~32767, gcc의 rand()는 0~2^31-1 이라 결과가 달라지므로
 * 직접 구현해서 어떤 환경에서도 같은 데이터가 나오게 한다.
 * --------------------------------------------------------- */
static unsigned int g_seed = 12345u;

void seed_random(unsigned int seed)
{
    g_seed = (seed == 0u) ? 1u : seed;   /* xorshift는 시드 0이면 안 됨 */
}

int next_rand(void)
{
    unsigned int x = g_seed;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    g_seed = x;
    return (int)(x & 0x7FFFFFFFu);
}