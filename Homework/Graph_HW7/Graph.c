#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"

/* ------------------------------------------------------------------
 * 구현 파일
 * 비교 횟수 규칙: 연산을 위해 "자료구조의 원소(행렬 칸 / 리스트 노드)를
 * 한 번 확인할 때마다" 비교 1회로 센다.
 * ------------------------------------------------------------------ */

 /* ==================================================================
  * 공통: 그래프 생성 및 테스트 대상 선택
  * ================================================================== */

  /* edges 배열로부터 존재 여부 확인용 임시 표를 만든다.
   * (난수 생성/선택용 임시 공간이므로 메모리 측정 대상이 아니다) */
static void build_used_table(const Edge* edges, int edgeCount,
    unsigned char used[NUM_VERTICES][NUM_VERTICES])
{
    int i;
    memset(used, 0, NUM_VERTICES * NUM_VERTICES);
    for (i = 0; i < edgeCount; i++) {
        used[edges[i].u][edges[i].v] = 1;
        used[edges[i].v][edges[i].u] = 1; /* 무방향이므로 양쪽 표시 */
    }
}

void generate_random_edges(Edge* edges, int edgeCount)
{
    unsigned char used[NUM_VERTICES][NUM_VERTICES];
    int made = 0;

    memset(used, 0, sizeof(used));

    while (made < edgeCount) {
        int u = rand() % NUM_VERTICES;
        int v = rand() % NUM_VERTICES;

        if (u == v) continue;      /* 자기 자신 연결(self-loop) 금지 */
        if (used[u][v]) continue;  /* 중복 간선 금지 */

        used[u][v] = used[v][u] = 1;
        edges[made].u = u;
        edges[made].v = v;
        made++;
    }
}

void pick_test_targets(const Edge* edges, int edgeCount, TestTargets* t)
{
    unsigned char used[NUM_VERTICES][NUM_VERTICES];
    int u, v;

    build_used_table(edges, edgeCount, used);

    /* 삽입할 간선: 현재 그래프에 "없는" 서로 다른 두 정점 */
    do {
        u = rand() % NUM_VERTICES;
        v = rand() % NUM_VERTICES;
    } while (u == v || used[u][v]);
    t->insertEdge.u = u;
    t->insertEdge.v = v;

    /* 삭제할 간선: 현재 그래프에 "있는" 간선 중 하나 */
    t->deleteEdge = edges[rand() % edgeCount];

    /* 연결 여부 확인용 정점 쌍: 서로 다른 임의의 두 정점 (연결 여부는 무관) */
    do {
        u = rand() % NUM_VERTICES;
        v = rand() % NUM_VERTICES;
    } while (u == v);
    t->connectPair.u = u;
    t->connectPair.v = v;

    /* 인접 정점을 조회할 정점 */
    t->adjacentVertex = rand() % NUM_VERTICES;
}

/* ==================================================================
 * 인접 행렬
 * ================================================================== */

 /* 그래프 구성 단계: 비교 횟수 측정 대상이 아니므로 바로 값을 채운다 */
void matrix_build(MatrixGraph* g, const Edge* edges, int edgeCount)
{
    int i;
    memset(g, 0, sizeof(MatrixGraph));
    g->vertexCount = NUM_VERTICES;
    for (i = 0; i < edgeCount; i++) {
        g->matrix[edges[i].u][edges[i].v] = 1;
        g->matrix[edges[i].v][edges[i].u] = 1; /* 무방향: 대칭 */
    }
    g->edgeCount = edgeCount;
}

/* matrix[u][v] 한 칸만 확인하면 되므로 비교는 항상 1회 */
int matrix_has_edge(const MatrixGraph* g, int u, int v, long* cmp)
{
    (*cmp)++;
    return g->matrix[u][v] == 1;
}

/* 이미 있는 간선인지 확인(1회) 후, 없으면 양쪽 칸을 1로 설정.
 * 값을 대입하는 것은 "확인"이 아니므로 비교 횟수에 포함하지 않는다. */
int matrix_insert_edge(MatrixGraph* g, int u, int v, long* cmp)
{
    if (matrix_has_edge(g, u, v, cmp)) return 0; /* 중복 간선이면 삽입 안 함 */
    g->matrix[u][v] = 1;
    g->matrix[v][u] = 1;
    g->edgeCount++;
    return 1;
}

/* 존재 확인(1회) 후, 있으면 양쪽 칸을 0으로 설정 */
int matrix_delete_edge(MatrixGraph* g, int u, int v, long* cmp)
{
    if (!matrix_has_edge(g, u, v, cmp)) return 0; /* 없는 간선이면 삭제 불가 */
    g->matrix[u][v] = 0;
    g->matrix[v][u] = 0;
    g->edgeCount--;
    return 1;
}

/* u행 전체(100칸)를 확인해야 어떤 정점이 이웃인지 알 수 있다.
 * 이웃이 몇 개든 비교는 항상 100회 */
int matrix_get_adjacent(const MatrixGraph* g, int u, int* out, long* cmp)
{
    int v, count = 0;
    for (v = 0; v < NUM_VERTICES; v++) {
        (*cmp)++;
        if (g->matrix[u][v] == 1) out[count++] = v;
    }
    return count;
}

/* 행렬 전체 + 부가 정보(vertexCount, edgeCount)가 모두 구조체에 들어 있다 */
size_t matrix_memory(const MatrixGraph* g)
{
    (void)g;
    return sizeof(MatrixGraph);
}

/* ==================================================================
 * 인접 리스트
 * ================================================================== */

 /* 정점 u의 리스트 맨 앞에 v 노드를 추가 (O(1), 비교 없음) */
static void add_head(ListGraph* g, int u, int v)
{
    ListNode* node = (ListNode*)malloc(sizeof(ListNode));
    if (node == NULL) {
        fprintf(stderr, "메모리 할당 실패\n");
        exit(1);
    }
    node->vertex = v;
    node->next = g->head[u];
    g->head[u] = node;
    g->nodeCount++;
}

/* 정점 u의 리스트에서 v 노드를 찾아 삭제한다.
 * 노드를 하나 확인할 때마다 비교 1회. 삭제에 성공하면 1 반환 */
static int remove_node(ListGraph* g, int u, int v, long* cmp)
{
    ListNode* prev = NULL;
    ListNode* cur = g->head[u];

    while (cur != NULL) {
        (*cmp)++;
        if (cur->vertex == v) {
            if (prev == NULL) g->head[u] = cur->next; /* 첫 노드를 삭제 */
            else              prev->next = cur->next; /* 중간/끝 노드 삭제 */
            free(cur);
            g->nodeCount--;
            return 1;
        }
        prev = cur;
        cur = cur->next;
    }
    return 0;
}

/* 그래프 구성 단계: 비교 횟수 측정 대상이 아님 */
void list_build(ListGraph* g, const Edge* edges, int edgeCount)
{
    int i;
    memset(g, 0, sizeof(ListGraph)); /* head[]를 모두 NULL로 초기화 */
    g->vertexCount = NUM_VERTICES;
    for (i = 0; i < edgeCount; i++) {
        add_head(g, edges[i].u, edges[i].v);
        add_head(g, edges[i].v, edges[i].u); /* 무방향: 양쪽 리스트에 저장 */
    }
    g->edgeCount = edgeCount;
}

void list_free(ListGraph* g)
{
    int i;
    for (i = 0; i < NUM_VERTICES; i++) {
        ListNode* cur = g->head[i];
        while (cur != NULL) {
            ListNode* next = cur->next;
            free(cur);
            cur = next;
        }
        g->head[i] = NULL;
    }
    g->nodeCount = 0;
}

/* u의 리스트를 앞에서부터 확인. 찾으면 거기서 중단, 없으면 끝까지 확인 */
int list_has_edge(const ListGraph* g, int u, int v, long* cmp)
{
    const ListNode* cur = g->head[u];
    while (cur != NULL) {
        (*cmp)++;
        if (cur->vertex == v) return 1;
        cur = cur->next;
    }
    return 0;
}

/* 중복 확인을 위해 u의 리스트를 끝까지 확인한 뒤(= 비교),
 * 없으면 양쪽 리스트의 맨 앞에 노드를 추가한다(추가 자체는 비교 없음) */
int list_insert_edge(ListGraph* g, int u, int v, long* cmp)
{
    if (list_has_edge(g, u, v, cmp)) return 0; /* 이미 있으면 삽입 안 함 */
    add_head(g, u, v);
    add_head(g, v, u);
    g->edgeCount++;
    return 1;
}

/* 양쪽 리스트(u의 리스트, v의 리스트)에서 각각 노드를 찾아야 하므로
 * 두 번의 탐색에서 확인한 노드 수를 모두 비교 횟수로 센다.
 * (행렬은 두 칸을 직접 접근하지만, 리스트는 두 곳을 모두 탐색해야 한다) */
int list_delete_edge(ListGraph* g, int u, int v, long* cmp)
{
    if (!remove_node(g, u, v, cmp)) return 0; /* 간선이 없으면 종료 */
    remove_node(g, v, u, cmp);
    g->edgeCount--;
    return 1;
}

/* u의 리스트에 저장된 노드만 순회: 비교 횟수 = u의 차수(degree) */
int list_get_adjacent(const ListGraph* g, int u, int* out, long* cmp)
{
    const ListNode* cur = g->head[u];
    int count = 0;
    while (cur != NULL) {
        (*cmp)++;
        out[count++] = cur->vertex;
        cur = cur->next;
    }
    return count;
}

/* 메모리 = 구조체 본체(head 배열 + 부가 정보) + 현재 할당된 노드들.
 * malloc 내부 관리 영역(헤더/정렬 패딩)은 환경마다 달라 제외하고,
 * sizeof(ListNode) 기준으로 계산한다. */
size_t list_memory(const ListGraph* g)
{
    return sizeof(ListGraph) + (size_t)g->nodeCount * sizeof(ListNode);
}

/* ==================================================================
 * 측정 / 출력
 * ================================================================== */

 /* 측정 순서: 메모리 -> 연결 확인 -> 인접 정점 -> 삽입 -> 삭제.
  * 메모리/조회 연산은 그래프를 바꾸지 않으므로 "원본 그래프" 기준이고,
  * 삽입/삭제는 그래프를 바꾸므로 마지막에 수행한다. */
void measure_matrix(MatrixGraph* g, const TestTargets* t, Result* r)
{
    memset(r, 0, sizeof(Result));

    r->memory = matrix_memory(g);

    r->connected = matrix_has_edge(g, t->connectPair.u, t->connectPair.v,
        &r->connectCmp);
    r->adjacentCount = matrix_get_adjacent(g, t->adjacentVertex,
        r->adjacent, &r->adjacentCmp);

    matrix_insert_edge(g, t->insertEdge.u, t->insertEdge.v, &r->insertCmp);
    matrix_delete_edge(g, t->deleteEdge.u, t->deleteEdge.v, &r->deleteCmp);
}

void measure_list(ListGraph* g, const TestTargets* t, Result* r)
{
    memset(r, 0, sizeof(Result));

    r->memory = list_memory(g);

    r->connected = list_has_edge(g, t->connectPair.u, t->connectPair.v,
        &r->connectCmp);
    r->adjacentCount = list_get_adjacent(g, t->adjacentVertex,
        r->adjacent, &r->adjacentCmp);

    list_insert_edge(g, t->insertEdge.u, t->insertEdge.v, &r->insertCmp);
    list_delete_edge(g, t->deleteEdge.u, t->deleteEdge.v, &r->deleteCmp);
}

void print_result(const char* caseName, const Result* r)
{
    int i;
    printf("%s\n", caseName);
    printf("Memory                         : %zu Bytes\n", r->memory);
    printf("Edge insertion comparisons     : %ld\n", r->insertCmp);
    printf("Edge deletion comparisons      : %ld\n", r->deleteCmp);
    printf("Connectivity check comparisons : %ld (%s)\n", r->connectCmp,
        r->connected ? "connected" : "not connected");
    printf("Adjacent vertices comparisons  : %ld (%d adjacent vertices)\n",
        r->adjacentCmp, r->adjacentCount);

    /* 인접 정점이 많으면 한 줄이 너무 길어지므로 15개마다 줄을 바꿔 출력 */
    printf("Adjacent vertices              :");
    if (r->adjacentCount == 0) printf(" (none)");
    for (i = 0; i < r->adjacentCount; i++) {
        if (i % 15 == 0) printf("\n    ");
        printf("%3d", r->adjacent[i]);
    }
    printf("\n\n");
}

/* 4가지 케이스를 한 표에 모아서 출력 (열 폭을 맞춰 비교하기 쉽게 한다) */
void print_summary(const Result results[4])
{
    int i;
    printf("======================== Summary ========================\n");
    printf("%-26s %12s %12s %12s %12s\n", "",
        "Sparse-Mat", "Sparse-List", "Dense-Mat", "Dense-List");
    printf("----------------------------------------------------------------------------\n");

    printf("%-26s", "Memory (Bytes)");
    for (i = 0; i < 4; i++) printf(" %12zu", results[i].memory);
    printf("\n%-26s", "Insertion comparisons");
    for (i = 0; i < 4; i++) printf(" %12ld", results[i].insertCmp);
    printf("\n%-26s", "Deletion comparisons");
    for (i = 0; i < 4; i++) printf(" %12ld", results[i].deleteCmp);
    printf("\n%-26s", "Connectivity comparisons");
    for (i = 0; i < 4; i++) printf(" %12ld", results[i].connectCmp);
    printf("\n%-26s", "Adjacent comparisons");
    for (i = 0; i < 4; i++) printf(" %12ld", results[i].adjacentCmp);
    printf("\n%-26s", "(adjacent vertex count)");
    for (i = 0; i < 4; i++) printf(" %12d", results[i].adjacentCount);
    printf("\n");
}