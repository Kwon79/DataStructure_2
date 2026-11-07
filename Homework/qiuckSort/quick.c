#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "quick.h"

#ifdef _WIN32
#include <windows.h>
static double now_ms(void)
{
    LARGE_INTEGER freq, cnt;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&cnt);
    return (double)cnt.QuadPart * 1000.0 / (double)freq.QuadPart;
}
#else
#include <time.h>
static double now_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1e6;
}
#endif

#define MAX_VALUE 1000000

/* MSVC의 rand()는 최대 32767이므로 두 번 호출해 범위를 넓힌다. */
static int rand_range(int len)
{
    return (int)((((unsigned int)rand() << 15) | (unsigned int)rand()) % (unsigned int)len);
}

void generate_data(int arr[], int n)
{
    int i;
    for (i = 0; i < n; i++) {
        arr[i] = rand_range(MAX_VALUE) + 1;
    }
}

static void swap_int(int* a, int* b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

/* 세 원소의 중앙값 위치를 반환. 값 비교는 모두 cmp에 포함한다. */
static int median_of_three(const int arr[], int low, int high, long long* cmp)
{
    int mid = low + (high - low) / 2;
    int a = arr[low], b = arr[mid], c = arr[high];

    (*cmp)++;
    if (a < b) {
        (*cmp)++;
        if (b < c) return mid;          /* a < b < c */
        (*cmp)++;
        if (a < c) return high;         /* a < c <= b */
        return low;                     /* c <= a < b */
    }
    else {
        (*cmp)++;
        if (a < c) return low;          /* b <= a < c */
        (*cmp)++;
        if (b < c) return high;         /* b < c <= a */
        return mid;                     /* c <= b <= a */
    }
}

/* 피벗 선택 방식만 다르고, 이후 분할 알고리즘은 모두 동일하다. */
static int choose_pivot(const int arr[], int low, int high,
    PivotType type, long long* cmp)
{
    switch (type) {
    case PIVOT_RANDOM:
        return low + rand_range(high - low + 1);
    case PIVOT_MEDIAN3:
        return median_of_three(arr, low, high, cmp);
    case PIVOT_FIRST:
    default:
        return low;
    }
}

/* 피벗을 low로 옮긴 뒤 분할하고, 피벗의 최종 위치를 반환한다. */
static int partition(int arr[], int low, int high, PivotType type, long long* cmp)
{
    int p = choose_pivot(arr, low, high, type, cmp);
    int pivot, i, j;

    swap_int(&arr[low], &arr[p]);
    pivot = arr[low];

    i = low + 1;
    j = high;
    while (1) {
        /* 데이터 값과 피벗 값의 비교만 카운트 (인덱스 검사는 제외) */
        while (i <= j && ((*cmp)++, arr[i] < pivot)) i++;
        while (i <= j && ((*cmp)++, arr[j] > pivot)) j--;
        if (i >= j) break;
        swap_int(&arr[i], &arr[j]);
        i++;
        j--;
    }
    swap_int(&arr[low], &arr[j]);
    return j;
}

static void qs_rec(int arr[], int low, int high, PivotType type, long long* cmp)
{
    int mid;
    if (low >= high) return;
    mid = partition(arr, low, high, type, cmp);
    qs_rec(arr, low, mid - 1, type, cmp);
    qs_rec(arr, mid + 1, high, type, cmp);
}

void quick_sort_recursive(int arr[], int n, PivotType type, long long* cmp)
{
    qs_rec(arr, 0, n - 1, type, cmp);
}

void quick_sort_iterative(int arr[], int n, PivotType type, long long* cmp)
{
    int* stack;
    int top = 0;

    if (n <= 1) return;

    /* 구간 하나당 (low, high) 두 칸. 동시에 최대 n개 구간이 쌓일 수 있다. */
    stack = (int*)malloc(sizeof(int) * 2 * (size_t)(n + 1));
    if (stack == NULL) return;

    stack[top++] = 0;
    stack[top++] = n - 1;

    while (top > 0) {
        int high = stack[--top];
        int low = stack[--top];
        int mid;

        if (low >= high) continue;
        mid = partition(arr, low, high, type, cmp);

        /* 재귀 버전과 같은 순서(왼쪽 먼저)로 처리되도록 오른쪽을 먼저 push */
        stack[top++] = mid + 1;
        stack[top++] = high;
        stack[top++] = low;
        stack[top++] = mid - 1;
    }
    free(stack);
}

int is_sorted(const int arr[], int n)
{
    int i;
    for (i = 1; i < n; i++) {
        if (arr[i - 1] > arr[i]) return 0;
    }
    return 1;
}

long long count_comparisons(const int origin[], int n, PivotType type)
{
    long long cmp = 0;
    int* copy = (int*)malloc(sizeof(int) * (size_t)n);
    if (copy == NULL) return -1;

    memcpy(copy, origin, sizeof(int) * (size_t)n);
    quick_sort_recursive(copy, n, type, &cmp);
    if (!is_sorted(copy, n)) cmp = -1;

    free(copy);
    return cmp;
}

void measure_time(const int origin[], int n, PivotType type, int repeat,
    double* rec_ms, double* iter_ms)
{
    int* copy = (int*)malloc(sizeof(int) * (size_t)n);
    long long dummy = 0;
    double t_rec = 0.0, t_iter = 0.0;
    int r;

    *rec_ms = *iter_ms = -1.0;
    if (copy == NULL) return;

    for (r = 0; r < repeat; r++) {
        double s;

        memcpy(copy, origin, sizeof(int) * (size_t)n);
        s = now_ms();
        quick_sort_recursive(copy, n, type, &dummy);
        t_rec += now_ms() - s;
        if (!is_sorted(copy, n)) { free(copy); return; }

        memcpy(copy, origin, sizeof(int) * (size_t)n);
        s = now_ms();
        quick_sort_iterative(copy, n, type, &dummy);
        t_iter += now_ms() - s;
        if (!is_sorted(copy, n)) { free(copy); return; }
    }

    *rec_ms = t_rec / repeat;
    *iter_ms = t_iter / repeat;
    free(copy);
}