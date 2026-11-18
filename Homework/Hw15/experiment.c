#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "experiment.h"

#ifdef _WIN32
#include <windows.h>
#endif

/* 데이터 값 범위: 이전 과제와 동일 (0 ~ 999999, 중복 허용) */
#define VALUE_RANGE 1000000

/* ------------------------------------------------------------------
 * 공통 측정 상태
 * ------------------------------------------------------------------ */
static long long g_cmp = 0;        /* 데이터 값 비교 횟수 */
static long long g_move = 0;       /* 데이터 값 이동(대입) 횟수 */
static int g_depth = 0;            /* 현재 재귀 깊이 */
static int g_max_depth = 0;        /* 최대 재귀 깊이 */
static long long g_nodes = 0;      /* 현재 AVL 노드 수 */
static long long g_nodes_peak = 0; /* 최대 AVL 노드 수 */

static void reset_counters(void)
{
    g_cmp = 0;
    g_move = 0;
    g_depth = 0;
    g_max_depth = 0;
    g_nodes = 0;
    g_nodes_peak = 0;
}

static void enter_call(void)
{
    g_depth++;
    if (g_depth > g_max_depth) g_max_depth = g_depth;
}

static void leave_call(void)
{
    g_depth--;
}

/* ------------------------------------------------------------------
 * 메모리 계산 기준 (README 참고, x64 기준)
 *  - 자료구조 크기 + 재귀 호출 스택(최대 깊이 x 프레임 크기)
 *  - 프레임 크기 = 매개변수/지역변수 sizeof 합 + 반환 주소 sizeof(void *)
 * ------------------------------------------------------------------ */

 /* 퀵 정렬 재귀 프레임: a, low, high, p + 반환 주소 */
#define QUICK_FRAME_BYTES \
    (sizeof(int *) + 3 * sizeof(int) + sizeof(void *))
/* partition() 프레임: a, low, high, mode, pivot, i, j, p + 피벗 선택 변수 + 반환 주소 */
#define QUICK_PART_BYTES \
    (sizeof(int *) + 10 * sizeof(int) + sizeof(void *))

/* AVL 재귀 프레임(삽입/삭제 중 큰 쪽 기준): node, key, found, 지역 포인터 2개 + 반환 주소 */
#define AVL_FRAME_BYTES \
    (2 * sizeof(void *) + sizeof(int *) + sizeof(int) + sizeof(void *))
/* 회전/재균형 호출 프레임(최하단에서 1회): 포인터 3개 + 정수 2개 + 반환 주소 */
#define AVL_ROT_BYTES \
    (3 * sizeof(void *) + 2 * sizeof(int) + sizeof(void *))

/* ------------------------------------------------------------------
 * 보조 함수
 * ------------------------------------------------------------------ */
 /* 데이터 값 비교 (3-way): 호출 1회 = 비교 1회로 계산 */
static int cmp3(int x, int y)
{
    g_cmp++;
    return (x > y) - (x < y);
}

/* x < y 비교: 호출 1회 = 비교 1회 */
static int less(int x, int y)
{
    g_cmp++;
    return x < y;
}

/* 교환 1회 = 대입 3회로 계산 (같은 위치끼리는 교환하지 않음) */
static void swap_int(int* x, int* y)
{
    int t;
    if (x == y) return;
    t = *x;
    *x = *y;
    *y = t;
    g_move += 3;
}

static int is_sorted(const int* a, int n)
{
    int i;
    for (i = 1; i < n; i++) {
        if (a[i - 1] > a[i]) return 0;
    }
    return 1;
}

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

/* 0 ~ 2^30-1 범위의 난수 (RAND_MAX 크기와 무관하게 동작) */
static int rand_int(void)
{
    return (((rand() & 0x7FFF) << 15) | (rand() & 0x7FFF));
}

/* ------------------------------------------------------------------
 * [1] 퀵 정렬 (피벗: 세 원소의 중앙값, Lomuto 분할)
 * ------------------------------------------------------------------ */
static int median3_index(const int* a, int low, int high)
{
    int mid = low + (high - low) / 2;
    int x = a[low], y = a[mid], z = a[high];

    if (less(x, y)) {
        if (less(y, z)) return mid;      /* x < y < z */
        if (less(x, z)) return high;     /* x < z <= y */
        return low;                      /* z <= x < y */
    }
    else {
        if (less(x, z)) return low;      /* y <= x < z */
        if (less(y, z)) return high;     /* y < z <= x */
        return mid;                      /* z <= y <= x */
    }
}

static int partition(int* a, int low, int high)
{
    int p = median3_index(a, low, high);
    int pivot, i, j;

    swap_int(&a[p], &a[high]);
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

static void quick_sort_rec(int* a, int low, int high)
{
    int p;

    enter_call();
    if (low < high) {
        p = partition(a, low, high);
        quick_sort_rec(a, low, p - 1);
        quick_sort_rec(a, p + 1, high);
    }
    leave_call();
}

/* 정렬된 배열에 삽입: 선형 탐색으로 위치를 찾고 뒤로 한 칸씩 이동 */
static void array_insert(int* a, int* n, int v)
{
    int pos = 0, i;

    while (pos < *n && cmp3(a[pos], v) < 0)   /* 처음으로 v 이상인 위치 */
        pos++;

    for (i = *n; i > pos; i--) {
        a[i] = a[i - 1];
        g_move++;
    }
    a[pos] = v;
    g_move++;
    (*n)++;
}

/* 정렬된 배열에서 삭제: 선형 탐색 후 뒤의 데이터를 앞으로 이동. 삭제했으면 1 */
static int array_delete(int* a, int* n, int v)
{
    int pos = 0, i, c = -1;

    while (pos < *n) {
        c = cmp3(a[pos], v);
        if (c >= 0) break;
        pos++;
    }
    if (pos >= *n || c != 0)
        return 0;                              /* 값이 없음 */

    for (i = pos; i < *n - 1; i++) {
        a[i] = a[i + 1];
        g_move++;
    }
    (*n)--;
    return 1;
}

ExpResult run_quick_experiment(const int* origin, int n, const Operation* ops, int num_ops, int* out)
{
    ExpResult r;
    double t0, t1, t2, t3;
    int cur = n, i;

    memcpy(out, origin, sizeof(int) * (size_t)n);   /* 복사 시간은 제외 */
    reset_counters();

    t0 = now_ms();
    quick_sort_rec(out, 0, n - 1);
    t1 = now_ms();

    r.ok = is_sorted(out, n);                       /* 검증 시간은 제외 */

    t2 = now_ms();
    for (i = 0; i < num_ops; i++) {
        if (ops[i].is_insert) array_insert(out, &cur, ops[i].value);
        else                  array_delete(out, &cur, ops[i].value);
    }
    t3 = now_ms();

    r.ok = r.ok && is_sorted(out, cur);
    r.time_ms = (t1 - t0) + (t3 - t2);
    r.comparisons = g_cmp;
    r.movements = g_move;
    r.final_count = cur;
    /* 배열(최대 n + num_ops개 할당) + 퀵 정렬 재귀 스택 */
    r.memory_bytes = sizeof(int) * (size_t)(n + num_ops)
        + (size_t)g_max_depth * QUICK_FRAME_BYTES + QUICK_PART_BYTES;
    return r;
}

/* ------------------------------------------------------------------
 * [2] AVL 트리 (중복 값은 노드의 count에 저장)
 * ------------------------------------------------------------------ */
typedef struct AvlNode {
    int key;
    int count;                 /* 같은 값의 개수 */
    int height;
    struct AvlNode* left;
    struct AvlNode* right;
} AvlNode;

static int node_height(const AvlNode* n)
{
    return n ? n->height : 0;
}

/* 높이/균형 인수 계산은 데이터 값과 무관한 제어 연산이므로 비교 횟수에 포함하지 않음 */
static void update_height(AvlNode* n)
{
    int hl = node_height(n->left), hr = node_height(n->right);
    n->height = 1 + (hl > hr ? hl : hr);
}

static AvlNode* rotate_right(AvlNode* y)
{
    AvlNode* x = y->left;
    y->left = x->right;
    x->right = y;
    update_height(y);
    update_height(x);
    return x;
}

static AvlNode* rotate_left(AvlNode* x)
{
    AvlNode* y = x->right;
    x->right = y->left;
    y->left = x;
    update_height(x);
    update_height(y);
    return y;
}

static AvlNode* rebalance(AvlNode* n)
{
    int bf;

    update_height(n);
    bf = node_height(n->left) - node_height(n->right);

    if (bf > 1) {                                           /* 왼쪽이 무거움 */
        if (node_height(n->left->left) < node_height(n->left->right))
            n->left = rotate_left(n->left);                 /* LR */
        return rotate_right(n);                             /* LL */
    }
    if (bf < -1) {                                          /* 오른쪽이 무거움 */
        if (node_height(n->right->right) < node_height(n->right->left))
            n->right = rotate_right(n->right);              /* RL */
        return rotate_left(n);                              /* RR */
    }
    return n;
}

static AvlNode* avl_insert(AvlNode* node, int key)
{
    int c;

    enter_call();
    if (node == NULL) {
        node = (AvlNode*)malloc(sizeof(AvlNode));
        if (node == NULL) {
            printf("메모리 할당 실패\n");
            exit(1);
        }
        node->key = key;
        g_move++;                       /* 데이터 값 저장 */
        node->count = 1;
        node->height = 1;
        node->left = node->right = NULL;
        g_nodes++;
        if (g_nodes > g_nodes_peak) g_nodes_peak = g_nodes;
        leave_call();
        return node;
    }

    c = cmp3(key, node->key);
    if (c < 0) {
        node->left = avl_insert(node->left, key);
    }
    else if (c > 0) {
        node->right = avl_insert(node->right, key);
    }
    else {
        node->count++;                  /* 중복 값: 개수만 증가 (구조 변화 없음) */
        leave_call();
        return node;
    }

    node = rebalance(node);
    leave_call();
    return node;
}

/* 서브트리에서 최솟값 노드를 떼어내고(*min에 반환) 남은 서브트리의 루트를 반환 (값 비교 없음) */
static AvlNode* detach_min(AvlNode* node, AvlNode** min)
{
    enter_call();
    if (node->left == NULL) {
        *min = node;
        leave_call();
        return node->right;
    }
    node->left = detach_min(node->left, min);
    node = rebalance(node);
    leave_call();
    return node;
}

/* key 한 개를 삭제. 삭제했으면 *found = 1 */
static AvlNode* avl_delete(AvlNode* node, int key, int* found)
{
    int c;

    enter_call();
    if (node == NULL) {
        *found = 0;
        leave_call();
        return NULL;
    }

    c = cmp3(key, node->key);
    if (c < 0) {
        node->left = avl_delete(node->left, key, found);
    }
    else if (c > 0) {
        node->right = avl_delete(node->right, key, found);
    }
    else {
        AvlNode* l, * r, * m;

        *found = 1;
        if (node->count > 1) {          /* 중복 값: 개수만 감소 */
            node->count--;
            leave_call();
            return node;
        }

        l = node->left;
        r = node->right;
        free(node);
        g_nodes--;

        if (l == NULL) { leave_call(); return r; }
        if (r == NULL) { leave_call(); return l; }

        /* 자식이 둘: 오른쪽 서브트리의 최솟값 노드를 떼어 그 자리에 연결 (포인터만 변경) */
        r = detach_min(r, &m);
        m->left = l;
        m->right = r;
        m = rebalance(m);
        leave_call();
        return m;
    }

    node = rebalance(node);
    leave_call();
    return node;
}

/* 중위 순회: 중복 개수만큼 풀어서 out에 오름차순으로 저장 */
static void avl_inorder(const AvlNode* node, int* out, int* idx)
{
    int k;

    if (node == NULL) return;
    avl_inorder(node->left, out, idx);
    for (k = 0; k < node->count; k++)
        out[(*idx)++] = node->key;
    avl_inorder(node->right, out, idx);
}

static void avl_free(AvlNode* node)
{
    if (node == NULL) return;
    avl_free(node->left);
    avl_free(node->right);
    free(node);
}

ExpResult run_avl_experiment(const int* origin, int n, const Operation* ops, int num_ops, int* out)
{
    ExpResult r;
    AvlNode* root = NULL;
    double t0, t1, t2, t3;
    int cur = n, i, idx, found;

    reset_counters();

    t0 = now_ms();
    for (i = 0; i < n; i++)
        root = avl_insert(root, origin[i]);
    t1 = now_ms();

    idx = 0;                                        /* 중위 순회 검증 시간은 제외 */
    avl_inorder(root, out, &idx);
    r.ok = (idx == n) && is_sorted(out, idx);

    t2 = now_ms();
    for (i = 0; i < num_ops; i++) {
        if (ops[i].is_insert) {
            root = avl_insert(root, ops[i].value);
            cur++;
        }
        else {
            found = 0;
            root = avl_delete(root, ops[i].value, &found);
            if (found) cur--;
        }
    }
    t3 = now_ms();

    idx = 0;                                        /* 최종 상태 확인 (시간 제외) */
    avl_inorder(root, out, &idx);
    r.ok = r.ok && (idx == cur) && is_sorted(out, idx);

    r.time_ms = (t1 - t0) + (t3 - t2);
    r.comparisons = g_cmp;
    r.movements = g_move;
    r.final_count = idx;
    /* 최대 노드 수 x 노드 크기 + 루트 포인터 + 재귀 스택 + 회전 프레임 */
    r.memory_bytes = (size_t)g_nodes_peak * sizeof(AvlNode) + sizeof(AvlNode*)
        + (size_t)g_max_depth * AVL_FRAME_BYTES + AVL_ROT_BYTES;

    avl_free(root);
    return r;
}

/* ------------------------------------------------------------------
 * 실험 실행 및 출력
 * ------------------------------------------------------------------ */
static void print_row(const char* label, const char* a, const char* b)
{
    printf("%-18s %-16s %-16s\n", label, a, b);
}

static void print_result(int n, int num_ops, ExpResult q, ExpResult t, int same)
{
    char a[64], b[64];

    printf("Data Size : %d\n", n);
    printf("Operations: %d\n\n", num_ops);
    print_row("", "Quick Sort", "AVL Tree");
    printf("------------------------------------------------\n");

    sprintf(a, "%.4f ms", q.time_ms);
    sprintf(b, "%.4f ms", t.time_ms);
    print_row("Total Time", a, b);

    sprintf(a, "%lld", q.comparisons);
    sprintf(b, "%lld", t.comparisons);
    print_row("Comparisons", a, b);

    sprintf(a, "%lld", q.movements);
    sprintf(b, "%lld", t.movements);
    print_row("Data Movements", a, b);

    sprintf(a, "%zu Bytes", q.memory_bytes);
    sprintf(b, "%zu Bytes", t.memory_bytes);
    print_row("Memory Usage", a, b);

    printf("\n");
    sprintf(a, "%d", q.final_count);
    sprintf(b, "%d", t.final_count);
    print_row("Final Data Count", a, b);

    printf("\nVerification : %s\n\n",
        (q.ok && t.ok && same) ? "OK (sorted, both results identical)" : "FAILED");
}

void run_experiment(void)
{
    const int sizes[] = { 1000, 2000, 4000, 8000, 16000, 20000 };
    const int num_sizes = (int)(sizeof(sizes) / sizeof(sizes[0]));
    int s, i, n, num_ops, same;
    int* origin, * outq, * outa;
    Operation* ops;
    ExpResult q, t;

    srand((unsigned int)time(NULL));

    for (s = 0; s < num_sizes; s++) {
        n = sizes[s];
        num_ops = n / 5;                              /* n의 20% */

        origin = (int*)malloc(sizeof(int) * (size_t)n);
        outq = (int*)malloc(sizeof(int) * (size_t)(n + num_ops));
        outa = (int*)malloc(sizeof(int) * (size_t)(n + num_ops));
        ops = (Operation*)malloc(sizeof(Operation) * (size_t)num_ops);
        if (!origin || !outq || !outa || !ops) {
            printf("메모리 할당 실패\n");
            exit(1);
        }

        /* 난수 생성은 측정 시간에서 제외: 데이터와 연산 목록을 미리 만든다 */
        for (i = 0; i < n; i++)
            origin[i] = rand_int() % VALUE_RANGE;
        for (i = 0; i < num_ops; i++) {
            ops[i].is_insert = (rand_int() % 2 == 0);   /* 짝수면 삽입, 홀수면 삭제 */
            ops[i].value = rand_int() % VALUE_RANGE;    /* 값은 별도의 난수 */
        }

        q = run_quick_experiment(origin, n, ops, num_ops, outq);
        t = run_avl_experiment(origin, n, ops, num_ops, outa);

        same = (q.final_count == t.final_count)
            && (memcmp(outq, outa, sizeof(int) * (size_t)q.final_count) == 0);

        print_result(n, num_ops, q, t, same);

        free(origin);
        free(outq);
        free(outa);
        free(ops);
    }
}