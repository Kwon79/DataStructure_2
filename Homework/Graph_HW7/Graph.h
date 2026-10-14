#include <stddef.h>

/* ------------------------------------------------------------------
 * 과제 07: 인접 행렬과 인접 리스트의 성능 비교
 * 헤더 파일: 상수, 구조체, 함수 선언
 * ------------------------------------------------------------------ */

#define NUM_VERTICES 100   /* 정점 수 (번호 0 ~ 99) */
#define SPARSE_EDGES 100   /* 희소 그래프 간선 수 */
#define DENSE_EDGES  4000  /* 밀집 그래프 간선 수 */

 /* 무방향 간선 하나 (u - v) */
typedef struct {
    int u;
    int v;
} Edge;

/* ===== 인접 행렬 ===== */
typedef struct {
    int matrix[NUM_VERTICES][NUM_VERTICES]; /* 1: 연결, 0: 비연결 */
    int vertexCount;                        /* 부가 정보: 정점 수 */
    int edgeCount;                          /* 부가 정보: 간선 수 */
} MatrixGraph;

/* ===== 인접 리스트 ===== */
/* 연결 리스트의 노드: 인접한 정점 번호 하나를 저장 */
typedef struct ListNode {
    int vertex;
    struct ListNode* next;
} ListNode;

typedef struct {
    ListNode* head[NUM_VERTICES]; /* 정점별 연결 리스트의 시작 포인터 */
    int vertexCount;              /* 부가 정보: 정점 수 */
    int edgeCount;                /* 부가 정보: 간선 수 */
    int nodeCount;                /* 현재 할당된 노드 수 (메모리 계산용) */
} ListGraph;

/* 한 번의 테스트에서 사용할 대상들.
 * 인접 행렬/리스트에 "동일한" 정점 쌍을 적용하기 위해 미리 한 번만 뽑는다. */
typedef struct {
    Edge insertEdge;     /* 삽입할 간선 (그래프에 없는 정점 쌍) */
    Edge deleteEdge;     /* 삭제할 간선 (그래프에 있는 간선) */
    Edge connectPair;    /* 연결 여부를 확인할 정점 쌍 */
    int  adjacentVertex; /* 인접 정점을 조회할 정점 */
} TestTargets;

/* 한 케이스(그래프 종류 x 표현 방식)의 측정 결과 */
typedef struct {
    size_t memory;                    /* 자료구조 메모리 (Byte) */
    long insertCmp;                   /* 간선 삽입 비교 횟수 */
    long deleteCmp;                   /* 간선 삭제 비교 횟수 */
    long connectCmp;                  /* 연결 여부 확인 비교 횟수 */
    long adjacentCmp;                 /* 인접 정점 조회 비교 횟수 */
    int  connected;                   /* 연결 여부 확인 결과 (1/0) */
    int  adjacent[NUM_VERTICES];      /* 조회된 인접 정점 목록 */
    int  adjacentCount;               /* 인접 정점 개수 */
} Result;

/* ---------- 그래프 생성 / 테스트 대상 선택 (공통) ---------- */
/* 중복/자기 자신 연결 없이 edgeCount개의 랜덤 간선을 edges에 생성 */
void generate_random_edges(Edge* edges, int edgeCount);
/* 테스트에 사용할 정점/간선들을 랜덤하게 선택 */
void pick_test_targets(const Edge* edges, int edgeCount, TestTargets* t);

/* ---------- 인접 행렬 ---------- */
void   matrix_build(MatrixGraph* g, const Edge* edges, int edgeCount);
int    matrix_has_edge(const MatrixGraph* g, int u, int v, long* cmp);
int    matrix_insert_edge(MatrixGraph* g, int u, int v, long* cmp);
int    matrix_delete_edge(MatrixGraph* g, int u, int v, long* cmp);
int    matrix_get_adjacent(const MatrixGraph* g, int u, int* out, long* cmp);
size_t matrix_memory(const MatrixGraph* g);

/* ---------- 인접 리스트 ---------- */
void   list_build(ListGraph* g, const Edge* edges, int edgeCount);
void   list_free(ListGraph* g);
int    list_has_edge(const ListGraph* g, int u, int v, long* cmp);
int    list_insert_edge(ListGraph* g, int u, int v, long* cmp);
int    list_delete_edge(ListGraph* g, int u, int v, long* cmp);
int    list_get_adjacent(const ListGraph* g, int u, int* out, long* cmp);
size_t list_memory(const ListGraph* g);

/* ---------- 측정 / 출력 ---------- */
void measure_matrix(MatrixGraph* g, const TestTargets* t, Result* r);
void measure_list(ListGraph* g, const TestTargets* t, Result* r);
void print_result(const char* caseName, const Result* r);
/* 4가지 케이스(0:희소-행렬, 1:희소-리스트, 2:밀집-행렬, 3:밀집-리스트)를 표로 비교 */
void print_summary(const Result results[4]);