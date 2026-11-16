#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "heap.h"

#ifdef _WIN32
#include <windows.h>
#endif

/* ------------------------------------------------------------------
 * 공통 전역 상태 (측정용)
 * ------------------------------------------------------------------ */
static long long g_cmp = 0;      /* 숫자 비교 횟수 */
static int g_depth = 0;          /* 현재 재귀 깊이 */
static int g_max_depth = 0;      /* 최대 재귀 깊이 */

static void enter_call(void)
{
    g_depth++;
    if (g_depth > g_max_depth) g_max_depth = g_depth;
}

static void leave_call(void)
{
    g_depth--;
}

static void reset_counters(void)
{
    g_cmp = 0;
    g_depth = 0;
    g_max_depth = 0;
}

/* ------------------------------------------------------------------
 * 메모리 계산 기준 (README 참고)
 *  - 지역 변수/매개변수는 sizeof 합으로 계산
 *  - 함수 호출 1회당 반환 주소(sizeof(void *))를 더함
 *  - 재귀 함수는 (최대 재귀 깊이 x 프레임 크기)로 계산
 * ------------------------------------------------------------------ */

 /* 힙 정렬 (재귀 없음): heap_sort()의 i, tmp + sift_down()의 a, n, root, child, tmp + 반환 주소 */
#define HEAP_AUX_BYTES \
    (2 * sizeof(int) + sizeof(int *) + 4 * sizeof(int) + sizeof(void *))

/* 병합 정렬 재귀 프레임: a, tmp, left, right, mid + 반환 주소 */
#define MERGE_FRAME_BYTES \
    (2 * sizeof(int *) + 3 * sizeof(int) + sizeof(void *))
/* merge() 프레임: a, tmp, left, mid, right, i, j, k + 반환 주소 (가장 깊은 곳에서 1회 호출) */
#define MERGE_FN_BYTES \
    (2 * sizeof(int *) + 6 * sizeof(int) + sizeof(void *))

/* 퀵 정렬 재귀 프레임: a, low, high, p + 반환 주소 */
#define QUICK_FRAME_BYTES \
    (sizeof(int *) + 3 * sizeof(int) + sizeof(void *))
/* partition() 프레임: a, low, high, pivot, i, j + 피벗 선택 보조 변수(mid, r, ...) 포함 + 반환 주소 */
#define QUICK_PART_BYTES \
    (sizeof(int *) + 8 * sizeof(int) + sizeof(void *))

/* ------------------------------------------------------------------
 * 보조 함수
 * ------------------------------------------------------------------ */
static void swap_int(int* x, int* y)
{
    int t = *x;
    *x = *y;
    *y = t;
}

/* 비교 횟수를 세는 비교 함수: x < y 이면 1 */
static int less(int x, int y)
{
    g_cmp++;
    return x < y;
}

/* 비교 횟수를 세는 비교 함수: x <= y 이면 1 */
static int less_eq(int x, int y)
{
    g_cmp++;
    return x <= y;
}

static int is_sorted(const int* a, int n)
{
    int i;
    for (i = 1; i < n; i++) {
        if (a[i - 1] > a[i]) return 0;
    }
    return 1;
}

/* 고해상도 타이머 (ms) */
static double now_ms(void)
{
#ifdef _WIN32
    LARGE_INTEGER freq, cnt;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&cnt);
    return (double)cnt.QuadPart * 1000.0 / (double)freq.QuadPart;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1e6;
#endif
}

/* 0 ~ 2^30-1 범위의 난수 (RAND_MAX가 32767이든 2^31-1이든 동일하게 동작) */
static int rand_int(void)
{
    return (((rand() & 0x7FFF) << 15) | (rand() & 0x7FFF));
}

/* ------------------------------------------------------------------
 * 1. 힙 정렬 (최대 힙, 입력 배열 자체를 힙으로 사용)
 * ------------------------------------------------------------------ */
static void sift_down(int* a, int n, int root)
{
    int child;
    int tmp = a[root];

    while ((child = 2 * root + 1) < n) {
        /* 두 자식 중 큰 쪽 선택 */
        if (child + 1 < n && less(a[child], a[child + 1]))
            child++;
        /* 자식이 tmp보다 크면 끌어올림 */
        if (less(tmp, a[child])) {
            a[root] = a[child];
            root = child;
        }
        else {
            break;
        }
    }
    a[root] = tmp;
}

static void heap_sort_impl(int* a, int n)
{
    int i;

    /* 최대 힙 구성 */
    for (i = n / 2 - 1; i >= 0; i--)
        sift_down(a, n, i);

    /* 루트(최댓값)를 끝으로 보내고 힙 크기를 줄이며 복구 */
    for (i = n - 1; i > 0; i--) {
        swap_int(&a[0], &a[i]);
        sift_down(a, i, 0);
    }
}

SortResult heap_sort(int* a, int n)
{
    SortResult r;
    double t0, t1;

    reset_counters();
    t0 = now_ms();
    heap_sort_impl(a, n);
    t1 = now_ms();

    r.comparisons = g_cmp;
    r.time_ms = t1 - t0;
    r.memory_bytes = HEAP_AUX_BYTES;
    r.sorted_ok = is_sorted(a, n);
    return r;
}

/* ------------------------------------------------------------------
 * 2. 병합 정렬 (top-down, 임시 배열 1개 사용)
 * ------------------------------------------------------------------ */
static void merge(int* a, int* tmp, int left, int mid, int right)
{
    int i = left, j = mid + 1, k = left;

    while (i <= mid && j <= right) {
        if (less_eq(a[i], a[j])) tmp[k++] = a[i++];
        else                     tmp[k++] = a[j++];
    }
    while (i <= mid)   tmp[k++] = a[i++];
    while (j <= right) tmp[k++] = a[j++];

    for (k = left; k <= right; k++)
        a[k] = tmp[k];
}

static void merge_sort_rec(int* a, int* tmp, int left, int right)
{
    int mid;

    enter_call();
    if (left < right) {
        mid = left + (right - left) / 2;
        merge_sort_rec(a, tmp, left, mid);
        merge_sort_rec(a, tmp, mid + 1, right);
        merge(a, tmp, left, mid, right);
    }
    leave_call();
}

SortResult merge_sort(int* a, int n)
{
    SortResult r;
    double t0, t1;
    int* tmp;

    reset_counters();
    t0 = now_ms();
    tmp = (int*)malloc(sizeof(int) * n);   /* 임시 배열 할당 */
    if (tmp == NULL) {
        printf("메모리 할당 실패\n");
        exit(1);
    }
    merge_sort_rec(a, tmp, 0, n - 1);
    free(tmp);
    t1 = now_ms();

    r.comparisons = g_cmp;
    r.time_ms = t1 - t0;
    r.memory_bytes = sizeof(int) * (size_t)n
        + (size_t)g_max_depth * MERGE_FRAME_BYTES
        + MERGE_FN_BYTES;
    r.sorted_ok = is_sorted(a, n);
    return r;
}

/* ------------------------------------------------------------------
 * 3. 퀵 정렬 (피벗 선택 3가지, Lomuto 분할)
 * ------------------------------------------------------------------ */
enum { PIVOT_FIRST, PIVOT_RANDOM, PIVOT_MEDIAN3 };

/* [low, high]에서 피벗 인덱스를 선택 */
static int choose_pivot(int* a, int low, int high, int mode)
{
    int mid, x, y, z;

    if (mode == PIVOT_FIRST)
        return low;

    if (mode == PIVOT_RANDOM)
        return low + rand_int() % (high - low + 1);

    /* 세 원소의 중앙값: 비교 2~3회 */
    mid = low + (high - low) / 2;
    x = a[low];
    y = a[mid];
    z = a[high];

    if (less(x, y)) {
        if (less(y, z)) return mid;          /* x < y < z */
        if (less(x, z)) return high;         /* x < z <= y */
        return low;                          /* z <= x < y */
    }
    else {
        if (less(x, z)) return low;          /* y <= x < z */
        if (less(y, z)) return high;         /* y < z <= x */
        return mid;                          /* z <= y <= x */
    }
}

static int partition(int* a, int low, int high, int mode)
{
    int p = choose_pivot(a, low, high, mode);
    int pivot, i, j;

    swap_int(&a[p], &a[high]);   /* 피벗을 끝으로 이동 */
    pivot = a[high];
    i = low - 1;

    for (j = low; j < high; j++) {
        if (less(a[j], pivot)) {
            i++;
            swap_int(&a[i], &a[j]);
        }
    }
    swap_int(&a[i + 1], &a[high]);
    return i + 1;
}

static void quick_sort_rec(int* a, int low, int high, int mode)
{
    int p;

    enter_call();
    if (low < high) {
        p = partition(a, low, high, mode);
        quick_sort_rec(a, low, p - 1, mode);
        quick_sort_rec(a, p + 1, high, mode);
    }
    leave_call();
}

static SortResult quick_sort_common(int* a, int n, int mode)
{
    SortResult r;
    double t0, t1;

    reset_counters();
    t0 = now_ms();
    quick_sort_rec(a, 0, n - 1, mode);
    t1 = now_ms();

    r.comparisons = g_cmp;
    r.time_ms = t1 - t0;
    r.memory_bytes = (size_t)g_max_depth * QUICK_FRAME_BYTES + QUICK_PART_BYTES;
    r.sorted_ok = is_sorted(a, n);
    return r;
}

SortResult quick_sort_first(int* a, int n) { return quick_sort_common(a, n, PIVOT_FIRST); }
SortResult quick_sort_random(int* a, int n) { return quick_sort_common(a, n, PIVOT_RANDOM); }
SortResult quick_sort_median3(int* a, int n) { return quick_sort_common(a, n, PIVOT_MEDIAN3); }

/* ------------------------------------------------------------------
 * 실험 실행 및 출력
 * ------------------------------------------------------------------ */
static void fill_random(int* a, int n)
{
    int i;
    for (i = 0; i < n; i++)
        a[i] = rand_int() % 1000000;   /* 중복 허용 */
}

static void print_row(const char* name, SortResult r)
{
    printf("%-15s %-14lld %-11.4f %-12zu%s\n",
        name, r.comparisons, r.time_ms, r.memory_bytes,
        r.sorted_ok ? "" : "  [정렬 오류!]");
}

void run_experiment(void)
{
    const int sizes[] = { 1000, 2000, 4000, 8000, 16000, 20000 };
    const int num_sizes = (int)(sizeof(sizes) / sizeof(sizes[0]));
    int s, n;
    int* origin, * work;

    srand((unsigned int)time(NULL));

    for (s = 0; s < num_sizes; s++) {
        n = sizes[s];
        origin = (int*)malloc(sizeof(int) * n);
        work = (int*)malloc(sizeof(int) * n);
        if (origin == NULL || work == NULL) {
            printf("메모리 할당 실패\n");
            exit(1);
        }

        fill_random(origin, n);

        printf("Data Size : %d\n\n", n);
        printf("%-15s %-14s %-11s %s\n", "Algorithm", "Comparisons", "Time(ms)", "Memory(Bytes)");
        printf("--------------------------------------------------------\n");

        /* 모든 알고리즘은 동일한 원본의 복사본 사용 (복사 시간은 측정 제외) */
        memcpy(work, origin, sizeof(int) * n);
        print_row("Heap Sort", heap_sort(work, n));

        memcpy(work, origin, sizeof(int) * n);
        print_row("Merge Sort", merge_sort(work, n));

        memcpy(work, origin, sizeof(int) * n);
        print_row("Quick (First)", quick_sort_first(work, n));

        memcpy(work, origin, sizeof(int) * n);
        print_row("Quick (Random)", quick_sort_random(work, n));

        memcpy(work, origin, sizeof(int) * n);
        print_row("Quick (Median3)", quick_sort_median3(work, n));

        printf("\n");
        free(origin);
        free(work);
    }
}