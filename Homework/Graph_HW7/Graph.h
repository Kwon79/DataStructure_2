#ifndef GRAPH_H
#define GRAPH_H

/* 과제 07: 인접 행렬과 인접 리스트의 성능 비교 (헤더 파일) */

#define MAX_VERTEX   100   /* 정점 수 (번호 0 ~ 99) */
#define SPARSE_EDGES 100   /* 희소 그래프 간선 수 */
#define DENSE_EDGES  4000  /* 밀집 그래프 간선 수 */

/* 인접 리스트의 노드: 인접 정점 번호(vertex)와 다음 노드를 가리키는 link */
typedef struct graphNode {
    int vertex;
    struct graphNode* link;
} graphNode;

/* 인접 행렬 그래프 */
typedef struct {
    int n;                                 /* 정점 개수 */
    int adjMatrix[MAX_VERTEX][MAX_VERTEX]; /* 1: 연결, 0: 연결 안 됨 */
} matrixGraph;

/* 인접 리스트 그래프 */
typedef struct {
    int n;                            /* 정점 개수 */
    graphNode* adjList_H[MAX_VERTEX]; /* 정점마다 인접 리스트의 헤드 포인터 */
} listGraph;

/* 비교 횟수를 세는 전역 변수.
 * 연산 직전에 0으로 만들고, 연산이 끝난 뒤 값을 읽는다. */
extern int compareCount;

/* ---------- 랜덤 간선 생성 ---------- */
/* 자기 자신 연결과 중복 간선이 없는 랜덤 간선 edgeCount개를 edges에 저장 */
void createEdges(int edges[][2], int edgeCount);

/* ---------- 인접 행렬 ---------- */
void createMatrixGraph(matrixGraph* g);
int  existsMatrixEdge(matrixGraph* g, int u, int v);
void insertMatrixEdge(matrixGraph* g, int u, int v);
void deleteMatrixEdge(matrixGraph* g, int u, int v);
int  printMatrixAdjacent(matrixGraph* g, int u);  /* 출력한 인접 정점 수를 반환 */
int  matrixMemory(matrixGraph* g);

/* ---------- 인접 리스트 ---------- */
void createListGraph(listGraph* g);
void freeListGraph(listGraph* g);
int  existsListEdge(listGraph* g, int u, int v);
void insertListEdge(listGraph* g, int u, int v);
void deleteListEdge(listGraph* g, int u, int v);
int  printListAdjacent(listGraph* g, int u);      /* 출력한 인접 정점 수를 반환 */
int  listMemory(listGraph* g);

#endif