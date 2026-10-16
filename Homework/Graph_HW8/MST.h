#pragma once
/* 과제 08: 최소 비용 신장 트리(MST) 알고리즘 비교 (헤더 파일) */

#define VERTEX_COUNT 50    /* 정점 수 (번호 0 ~ 49) */
#define SPARSE_EDGES 100   /* 희소 그래프 간선 수 */
#define DENSE_EDGES  600   /* 밀집 그래프 간선 수 */
#define MAX_WEIGHT   100   /* 간선 가중치는 1 ~ MAX_WEIGHT 사이의 양의 정수 */
#define INF          1000000  /* 인접 행렬에서 "간선 없음"을 나타내는 값 (무한대 대신 사용) */
#define REPEAT       1000  /* 실행시간 측정 시 MST를 반복해서 구하는 횟수 (아래 main.c 설명 참고) */

/* 간선 하나: 정점 u, v와 가중치 w */
typedef struct {
    int u;
    int v;
    int w;
} edge;

/* ===== 인접 리스트 ===== */
/* 리스트의 노드: 인접 정점 번호(vertex), 간선의 가중치(weight), 다음 노드(link) */
typedef struct listNode {
    int vertex;
    int weight;
    struct listNode* link;
} listNode;

typedef struct {
    int n;                                  /* 정점 개수 */
    int e;                                  /* 간선 개수 */
    listNode* adjList_H[VERTEX_COUNT];      /* 정점마다 인접 리스트의 헤드 포인터 */
} listGraph;

/* ===== 인접 행렬 ===== */
typedef struct {
    int n;                                      /* 정점 개수 */
    int e;                                      /* 간선 개수 */
    int adjMatrix[VERTEX_COUNT][VERTEX_COUNT];  /* 간선의 가중치 (간선이 없으면 INF) */
} matrixGraph;

/* ---------- 랜덤 그래프 생성 ---------- */
/* 연결 그래프가 되도록 중복/자기 자신 연결이 없는 랜덤 가중치 간선 edgeCount개를 만든다 */
void createEdges(edge edges[], int edgeCount);

/* ---------- 인접 행렬 ---------- */
void createMatrixGraph(matrixGraph* g);
void insertMatrixEdge(matrixGraph* g, int u, int v, int w);
int  isConnected(matrixGraph* g);   /* 모든 정점이 연결되어 있으면 1, 아니면 0 */

/* ---------- 인접 리스트 ---------- */
void createListGraph(listGraph* g);
void insertListEdge(listGraph* g, int u, int v, int w);
void freeListGraph(listGraph* g);

/* ---------- MST 알고리즘 ----------
 * 둘 다 MST에 포함된 간선을 mst[]에 저장하고, 전체 비용을 *totalCost에 저장하며,
 * MST에 포함된 간선 수를 반환한다. */
int kruskal(listGraph* g, edge mst[], int* totalCost);   /* 인접 리스트 + Kruskal */
int prim(matrixGraph* g, edge mst[], int* totalCost);    /* 인접 행렬 + Prim */

/* ---------- 메모리 사용량 (Byte) ---------- */
int kruskalMemory(listGraph* g);
int primMemory(matrixGraph* g);

/* ---------- 출력 ---------- */
void printMST(edge mst[], int count);