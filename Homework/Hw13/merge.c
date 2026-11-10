#include <stdlib.h>
#include <string.h>
#include "merge.h"
#include <windows.h>

static double now_ms(void)
{
    LARGE_INTEGER freq, cnt;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&cnt);
    return (double)cnt.QuadPart * 1000.0 / (double)freq.QuadPart;
}


#define MAX_VALUE 1000000

/* 정렬 한 번 동안 비교 횟수와 재귀 깊이를 모으는 내부용 구조체 */
typedef struct {
    long long cmp;      /* 숫자 비교 횟수 */
    int depth;          /* 현재 재귀 깊이 */
    int max_depth;      /* 가장 깊었던 재귀 깊이 */
} Ctx;

/* 재귀 호출 1회가 스택에 차지하는 크기(Byte) 추정값
   = 반환 주소 + 매개변수 + 지역변수 크기의 합 (패딩, 레지스터 저장 공간은 제외)
   qs_rec : 포인터 3개(반환 주소, arr, ctx) + int 3개(low, high, mid) + SortType
   ms_rec : 포인터 4개(반환 주소, arr, tmp, ctx) + int 3개(low, high, mid) */
#define QS_FRAME_BYTES ((int)(3 * sizeof(void *) + 3 * sizeof(int) + sizeof(SortType)))
#define MS_FRAME_BYTES ((int)(4 * sizeof(void *) + 3 * sizeof(int)))

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

int is_sorted(const int arr[], int n)
{
    int i;
    for (i = 1; i < n; i++) {
        if (arr[i - 1] > arr[i]) return 0;
    }
    return 1;
}

const char* sort_name(SortType type)
{
    switch (type) {
    case SORT_MERGE:         return "Merge Sort";
    case SORT_QUICK_FIRST:   return "Quick (First)";
    case SORT_QUICK_RANDOM:  return "Quick (Random)";
    case SORT_QUICK_MEDIAN3: return "Quick (Median3)";
    default:                 return "Unknown";
    }
}

/* ---------------------------- 병합 정렬 ---------------------------- */

/* arr[low..mid]와 arr[mid+1..high]를 병합한다. 값 비교 1회마다 cmp를 올린다. */
static void merge(int arr[], int tmp[], int low, int mid, int high, long long* cmp)
{
    int i = low, j = mid + 1, k = low;

    while (i <= mid && j <= high) {
        (*cmp)++;
        if (arr[i] <= arr[j]) tmp[k++] = arr[i++];
        else                  tmp[k++] = arr[j++];
    }
    while (i <= mid)  tmp[k++] = arr[i++];
    while (j <= high) tmp[k++] = arr[j++];

    memcpy(arr + low, tmp + low, sizeof(int) * (size_t)(high - low + 1));
}

static void ms_rec(int arr[], int tmp[], int low, int high, Ctx* c)
{
    c->depth++;
    if (c->depth > c->max_depth) c->max_depth = c->depth;

    if (low < high) {
        int mid = low + (high - low) / 2;
        ms_rec(arr, tmp, low, mid, c);
        ms_rec(arr, tmp, mid + 1, high, c);
        merge(arr, tmp, low, mid, high, &c->cmp);
    }
    c->depth--;
}

/* ---------------------------- 퀵 정렬 ---------------------------- */

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
    SortType type, long long* cmp)
{
    switch (type) {
    case SORT_QUICK_RANDOM:
        return low + rand_range(high - low + 1);
    case SORT_QUICK_MEDIAN3:
        return median_of_three(arr, low, high, cmp);
    case SORT_QUICK_FIRST:
    default:
        return low;
    }
}

/* 피벗을 low로 옮긴 뒤 분할하고, 피벗의 최종 위치를 반환한다. */
static int partition(int arr[], int low, int high, SortType type, long long* cmp)
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

static void qs_rec(int arr[], int low, int high, SortType type, Ctx* c)
{
    c->depth++;
    if (c->depth > c->max_depth) c->max_depth = c->depth;

    if (low < high) {
        int mid = partition(arr, low, high, type, &c->cmp);
        qs_rec(arr, low, mid - 1, type, c);
        qs_rec(arr, mid + 1, high, type, c);
    }
    c->depth--;
}

/* ---------------------------- 측정 ---------------------------- */

/* arr를 type 방식으로 정렬하고, 이 정렬이 사용한 추가 메모리(Byte)를 반환한다.
   (메모리 할당 실패 시 -1)
   - 병합 정렬: 임시 배열 n * sizeof(int) + 재귀 최대 깊이 * 호출 1회 크기
   - 퀵 정렬  : 재귀 최대 깊이 * 호출 1회 크기 (별도 배열 없음) */
static long long run_sort(int arr[], int n, SortType type, Ctx* c)
{
    if (type == SORT_MERGE) {
        int* tmp = (int*)malloc(sizeof(int) * (size_t)n);
        if (tmp == NULL) return -1;
        ms_rec(arr, tmp, 0, n - 1, c);
        free(tmp);
        return (long long)n * (long long)sizeof(int)
            + (long long)c->max_depth * MS_FRAME_BYTES;
    }

    qs_rec(arr, 0, n - 1, type, c);
    return (long long)c->max_depth * QS_FRAME_BYTES;
}

SortStat measure_sort(const int origin[], int n, SortType type, int repeat)
{
    SortStat st;
    double total = 0.0;
    int* copy;
    int r;

    st.comparisons = -1;
    st.time_ms = 0.0;
    st.memory = 0;

    copy = (int*)malloc(sizeof(int) * (size_t)n);
    if (copy == NULL) return st;

    for (r = 0; r < repeat; r++) {
        Ctx c;
        long long mem;
        double start;

        c.cmp = 0;
        c.depth = 0;
        c.max_depth = 0;

        memcpy(copy, origin, sizeof(int) * (size_t)n);   /* 복사 시간은 제외 */

        start = now_ms();
        mem = run_sort(copy, n, type, &c);
        total += now_ms() - start;

        if (mem < 0 || !is_sorted(copy, n)) {
            st.comparisons = -1;
            free(copy);
            return st;
        }
        if (r == 0) {
            st.comparisons = c.cmp;
            st.memory = mem;
        }
    }

    st.time_ms = total / repeat;
    free(copy);
    return st;
}