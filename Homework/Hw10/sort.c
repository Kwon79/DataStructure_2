#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sort.h"

/* ================= 정렬 알고리즘 + 카운터 ================= */

unsigned long long g_cmp = 0;
unsigned long long g_mov = 0;

void reset_counters(void)
{
    g_cmp = 0;
    g_mov = 0;
}

/* 교환 = 이동 3회 */
static void swap_data(Data* a, Data* b)
{
    Data tmp;
    MOVE(tmp, *a);
    MOVE(*a, *b);
    MOVE(*b, tmp);
}

/* 선택 정렬 */
void selection_sort(Data* a, int n)
{
    int i, j, min;
    for (i = 0; i < n - 1; i++) {
        min = i;
        for (j = i + 1; j < n; j++) {
            g_cmp++;
            if (a[j].key1 < a[min].key1) min = j;
        }
        if (min != i) swap_data(&a[i], &a[min]);   /* 같은 위치면 교환 생략 */
    }
}

/* 버블 정렬 (모든 순회 수행) */
void bubble_sort(Data* a, int n)
{
    int i, j;
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - 1 - i; j++) {
            g_cmp++;
            if (a[j].key1 > a[j + 1].key1) swap_data(&a[j], &a[j + 1]);
        }
    }
}

/* 버블 정렬 (조기 종료) */
void bubble_sort_early(Data* a, int n)
{
    int i, j, swapped;
    for (i = 0; i < n - 1; i++) {
        swapped = 0;
        for (j = 0; j < n - 1 - i; j++) {
            g_cmp++;
            if (a[j].key1 > a[j + 1].key1) {
                swap_data(&a[j], &a[j + 1]);
                swapped = 1;
            }
        }
        if (!swapped) break;
    }
}

/* 삽입 정렬 Case A: data[j].key1 > current.key1 (안정) */
void insertion_sort_gt(Data* a, int n)
{
    int i, j;
    Data cur;
    for (i = 1; i < n; i++) {
        MOVE(cur, a[i]);
        j = i - 1;
        while (j >= 0) {
            g_cmp++;
            if (a[j].key1 > cur.key1) { MOVE(a[j + 1], a[j]); j--; }
            else break;
        }
        MOVE(a[j + 1], cur);
    }
}

/* 삽입 정렬 Case B: data[j].key1 >= current.key1 (불안정) */
void insertion_sort_ge(Data* a, int n)
{
    int i, j;
    Data cur;
    for (i = 1; i < n; i++) {
        MOVE(cur, a[i]);
        j = i - 1;
        while (j >= 0) {
            g_cmp++;
            if (a[j].key1 >= cur.key1) { MOVE(a[j + 1], a[j]); j--; }
            else break;
        }
        MOVE(a[j + 1], cur);
    }
}

/* ================= 유틸 ================= */

static unsigned int g_rng = 1;

void rng_seed(unsigned int s) { g_rng = s ? s : 1; }

unsigned int rng_next(void)
{
    g_rng ^= g_rng << 13;
    g_rng ^= g_rng >> 17;
    g_rng ^= g_rng << 5;
    return g_rng;
}

int rand_key(void) { return (int)(rng_next() % (MAX_KEY + 1)); }

void make_random(Data* a, int n)
{
    int i;
    for (i = 0; i < n; i++) {
        a[i].key1 = rand_key();
        a[i].key2 = rand_key();
        a[i].order = i;
    }
}

int is_sorted(const Data* a, int n)
{
    int i;
    for (i = 0; i + 1 < n; i++)
        if (a[i].key1 > a[i + 1].key1) return 0;
    return 1;
}

int stability_violations(const Data* a, int n)
{
    int i, v = 0;
    for (i = 0; i + 1 < n; i++)
        if (a[i].key1 == a[i + 1].key1 && a[i].order > a[i + 1].order) v++;
    return v;
}

int duplicate_pairs(const Data* a, int n)
{
    int i, c = 0;
    for (i = 0; i + 1 < n; i++)
        if (a[i].key1 == a[i + 1].key1) c++;
    return c;
}

int cmp_asc(const void* p, const void* q)
{
    const Data* x = (const Data*)p, * y = (const Data*)q;
    if (x->key1 != y->key1) return (x->key1 < y->key1) ? -1 : 1;
    return (x->order < y->order) ? -1 : (x->order > y->order);
}

/* ================= 실험 ================= */


typedef struct { const char* name; SortFn fn; } Algo;

static const Algo ALGOS[4] = {
    { "Selection Sort",                  selection_sort    },
    { "Bubble Sort",                     bubble_sort       },
    { "Bubble Sort (Early Termination)", bubble_sort_early },
    { "Insertion Sort",                  insertion_sort_gt }
};

static Data g_work[N];           /* 정렬에 사용하는 작업용 복사본 */
static Data g_first_random[N];   /* 첫 시행 무작위 데이터 (이후 실험의 원본) */

/* 복사본으로 정렬 수행, 카운터 기록, 정렬 결과 검증 */
static int run_sort(SortFn fn, const Data* base, int n,
    unsigned long long* cmp, unsigned long long* mov)
{
    memcpy(g_work, base, sizeof(Data) * n);   /* 복사는 카운트에 포함하지 않음 */
    reset_counters();
    fn(g_work, n);
    *cmp = g_cmp; *mov = g_mov;
    return is_sorted(g_work, n);
}

/* ---------- 실험 1: 무작위 100회 ---------- */
static void experiment_random(void)
{
    static Data base[N];
    unsigned long long tc[4] = { 0 }, tm[4] = { 0 }, c, m;
    int t, k, all_ok[4] = { 1, 1, 1, 1 };

    for (t = 0; t < TRIALS; t++) {
        rng_seed(20240001u + 7919u * (unsigned)t);
        make_random(base, N);
        if (t == 0) memcpy(g_first_random, base, sizeof(base));
        for (k = 0; k < 4; k++) {   /* 모든 알고리즘이 동일한 base 사용 */
            if (!run_sort(ALGOS[k].fn, base, N, &c, &m)) all_ok[k] = 0;
            tc[k] += c; tm[k] += m;
        }
    }

    printf("Random Data\n");
    printf("Number of Trials : %d\n", TRIALS);
    for (k = 0; k < 4; k++) {
        printf("\n%s\n", ALGOS[k].name);
        printf("Total Comparisons   : %llu\n", tc[k]);
        printf("Average Comparisons : %.2f\n", (double)tc[k] / TRIALS);
        printf("Total Movements     : %llu\n", tm[k]);
        printf("Average Movements   : %.2f\n", (double)tm[k] / TRIALS);
        printf("Sorted Correctly    : %s (%d trials)\n", all_ok[k] ? "YES" : "NO", TRIALS);
    }
    printf("\n------------------------------------\n\n");
}

/* ---------- 실험 2: 입력 상태별 ---------- */
static void experiment_state(const char* title, const Data* base)
{
    unsigned long long c, m;
    int k, ok;
    printf("%s\n", title);
    for (k = 0; k < 4; k++) {
        ok = run_sort(ALGOS[k].fn, base, N, &c, &m);
        printf("\n%s\n", ALGOS[k].name);
        printf("Comparisons : %llu\n", c);
        printf("Movements   : %llu\n", m);
        printf("Sorted      : %s\n", ok ? "YES" : "NO");
    }
    printf("\n------------------------------------\n\n");
}

/* ---------- 실험 3: 안정성 ---------- */
static void print_order(const char* name, const Data* a, int n)
{
    int i;
    printf("  %-20s:", name);
    for (i = 0; i < n; i++) printf(" (%d,%d)", a[i].key1, a[i].key2);
    printf("\n");
}

typedef struct { const char* name; SortFn fn; int viol[3]; } StabCase;

static void experiment_stability(void)
{
    StabCase cs[4] = {
        { "Selection Sort",      selection_sort,    {0,0,0} },
        { "Bubble Sort",         bubble_sort,       {0,0,0} },
        { "Insertion Sort (>)",  insertion_sort_gt, {0,0,0} },
        { "Insertion Sort (>=)", insertion_sort_ge, {0,0,0} }
    };
    static Data dup[1000];
    Data small[4] = { {527,100,0}, {527,200,1}, {125,300,2}, {892,400,3} };
    unsigned long long c, m;
    int k, i, ok;

    /* 시험 데이터 2: key1이 0~99 범위 -> 중복이 매우 많음 (1000개) */
    rng_seed(777u);
    for (i = 0; i < 1000; i++) {
        dup[i].key1 = (int)(rng_next() % 100);
        dup[i].key2 = rand_key();
        dup[i].order = i;
    }

    printf("Stability Verification\n\n");
    printf("[Test 1] Example data from the assignment (key1, key2)\n");
    print_order("Original", small, 4);
    for (k = 0; k < 4; k++) {
        memcpy(g_work, small, sizeof(small));
        cs[k].fn(g_work, 4);
        cs[k].viol[0] = stability_violations(g_work, 4);
        print_order(cs[k].name, g_work, 4);
    }

    printf("\n[Test 2] 1000 items, key1 in 0..99 (many duplicates)\n");
    for (k = 0; k < 4; k++) {
        ok = run_sort(cs[k].fn, dup, 1000, &c, &m);
        cs[k].viol[1] = stability_violations(g_work, 1000);
        if (k == 0) printf("  adjacent equal-key pairs after sort: %d\n", duplicate_pairs(g_work, 1000));
        printf("  %-20s: sorted=%s, order-violations=%d, comparisons=%llu, movements=%llu\n",
            cs[k].name, ok ? "YES" : "NO", cs[k].viol[1], c, m);
    }

    printf("\n[Test 3] 10000 random items (data of trial #1)\n");
    for (k = 0; k < 4; k++) {
        ok = run_sort(cs[k].fn, g_first_random, N, &c, &m);
        cs[k].viol[2] = stability_violations(g_work, N);
        if (k == 0) printf("  adjacent equal-key pairs after sort: %d\n", duplicate_pairs(g_work, N));
        printf("  %-20s: sorted=%s, order-violations=%d\n", cs[k].name, ok ? "YES" : "NO", cs[k].viol[2]);
    }

    printf("\n------------------------------------\n\n");
    printf("Stability Verification (Summary)\n\n");
    for (k = 0; k < 4; k++) {
        int total = cs[k].viol[0] + cs[k].viol[1] + cs[k].viol[2];
        printf("%-21s: %s (violations: %d)\n", cs[k].name,
            total == 0 ? "Stable" : "Unstable", total);
    }
}

/* ---------- 전체 실행 ---------- */
void run_all_experiments(void)
{
    static Data sorted[N], reversed[N], partial[N];
    int i, a, b, swaps;
    Data tmp;

    printf("Data Count : %d\n\n", N);

    /* 1) 무작위 100회 (첫 시행 데이터는 g_first_random에 저장) */
    experiment_random();

    /* 2) 나머지 3가지 상태는 첫 시행 무작위 데이터에서 파생 */
    memcpy(sorted, g_first_random, sizeof(sorted));
    qsort(sorted, N, sizeof(Data), cmp_asc);                 /* 오름차순 */

    for (i = 0; i < N; i++) reversed[i] = sorted[N - 1 - i]; /* 내림차순 */

    /* 일부 정렬: 정렬된 데이터에서 무작위 두 위치를 N/20 (=5%) 번 교환 */
    memcpy(partial, sorted, sizeof(partial));
    rng_seed(424242u);
    swaps = N / 20;
    for (i = 0; i < swaps; i++) {
        a = (int)(rng_next() % N);
        b = (int)(rng_next() % N);
        tmp = partial[a]; partial[a] = partial[b]; partial[b] = tmp;
    }

    experiment_state("Sorted Data", sorted);
    experiment_state("Reverse-Sorted Data", reversed);
    experiment_state("Partially Sorted Data (5% random swaps)", partial);

    /* 3) 안정성 */
    experiment_stability();
}