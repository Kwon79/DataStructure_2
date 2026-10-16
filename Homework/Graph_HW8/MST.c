#include <stdio.h>
#include <stdlib.h>
#include "mst.h"

/* ==================================================================
 * 랜덤 그래프 생성
 * ================================================================== */

void createEdges(edge edges[], int edgeCount)
{
    int used[VERTEX_COUNT][VERTEX_COUNT] = { 0 };  /* 이미 만든 간선 표시용 임시 배열 (메모리 측정 대상 아님) */
    int made = 0;                                  /* 지금까지 만든 간선 수 */
    int i, u, v;

    /* 1) 연결 그래프를 보장하기 위해 먼저 간선 49개짜리 트리를 만든다.
     *    정점 i를 이미 연결된 정점(0 ~ i-1) 중 하나에 이어 붙이면 모든 정점이 하나로 연결된다. */
    for (i = 1; i < VERTEX_COUNT; i++) {
        u = i;
        v = rand() % i;
        used[u][v] = 1;
        used[v][u] = 1;
        edges[made].u = u;
        edges[made].v = v;
        edges[made].w = rand() % MAX_WEIGHT + 1;   /* 1 ~ MAX_WEIGHT */
        made++;
    }

    /* 2) 나머지 간선은 아무 두 정점이나 골라서 채운다 */
    while (made < edgeCount) {
        u = rand() % VERTEX_COUNT;
        v = rand() % VERTEX_COUNT;

        if (u == v) continue;           /* 자기 자신과 연결 금지 */
        if (used[u][v] == 1) continue;  /* 중복 간선 금지 */

        used[u][v] = 1;
        used[v][u] = 1;                 /* 무방향이므로 양쪽 표시 */
        edges[made].u = u;
        edges[made].v = v;
        edges[made].w = rand() % MAX_WEIGHT + 1;
        made++;
    }
}

/* ==================================================================
 * 인접 행렬
 * ================================================================== */

 /* 간선이 하나도 없는 인접 행렬을 만든다 (간선 없음 = INF) */
void createMatrixGraph(matrixGraph* g)
{
    int i, j;
    g->n = VERTEX_COUNT;
    g->e = 0;
    for (i = 0; i < VERTEX_COUNT; i++)
        for (j = 0; j < VERTEX_COUNT; j++)
            g->adjMatrix[i][j] = INF;
}

void insertMatrixEdge(matrixGraph* g, int u, int v, int w)
{
    g->adjMatrix[u][v] = w;
    g->adjMatrix[v][u] = w;   /* 무방향 그래프이므로 대칭으로 저장 */
    g->e++;
}

/* 깊이 우선 탐색으로 정점 v에서 갈 수 있는 정점을 모두 방문 표시한다 */
static void dfs(matrixGraph* g, int v, int visited[])
{
    int w;
    visited[v] = 1;
    for (w = 0; w < g->n; w++)
        if (g->adjMatrix[v][w] != INF && visited[w] == 0)
            dfs(g, w, visited);
}

/* 0번 정점에서 출발해 모든 정점을 방문할 수 있으면 연결 그래프 */
int isConnected(matrixGraph* g)
{
    int visited[VERTEX_COUNT] = { 0 };
    int i;

    dfs(g, 0, visited);
    for (i = 0; i < g->n; i++)
        if (visited[i] == 0) return 0;   /* 방문하지 못한 정점이 있으면 연결 그래프가 아님 */
    return 1;
}

/* ==================================================================
 * 인접 리스트
 * ================================================================== */

 /* 간선이 하나도 없는 인접 리스트를 만든다 (헤드 포인터는 모두 NULL) */
void createListGraph(listGraph* g)
{
    int i;
    g->n = VERTEX_COUNT;
    g->e = 0;
    for (i = 0; i < VERTEX_COUNT; i++)
        g->adjList_H[i] = NULL;
}

/* u의 인접 리스트 맨 앞에 (v, w) 노드를 추가한다 */
static void addFirstNode(listGraph* g, int u, int v, int w)
{
    listNode* newNode = (listNode*)malloc(sizeof(listNode));
    newNode->vertex = v;
    newNode->weight = w;
    newNode->link = g->adjList_H[u];   /* 새 노드가 기존 첫 노드를 가리키게 한다 */
    g->adjList_H[u] = newNode;         /* 새 노드를 첫 노드로 만든다 */
}

void insertListEdge(listGraph* g, int u, int v, int w)
{
    addFirstNode(g, u, v, w);
    addFirstNode(g, v, u, w);   /* 무방향 그래프이므로 반대쪽 리스트에도 저장 */
    g->e++;
}

void freeListGraph(listGraph* g)
{
    int i;
    listNode* p;
    listNode* next;

    for (i = 0; i < g->n; i++) {
        p = g->adjList_H[i];
        while (p != NULL) {
            next = p->link;   /* free 하기 전에 다음 노드를 기억해 둔다 */
            free(p);
            p = next;
        }
        g->adjList_H[i] = NULL;
    }
}

/* ==================================================================
 * Kruskal 알고리즘 (인접 리스트)
 * ================================================================== */

 /* qsort에서 간선을 가중치 오름차순으로 정렬하기 위한 비교 함수 */
static int compareEdge(const void* a, const void* b)
{
    return ((const edge*)a)->w - ((const edge*)b)->w;
}

/* Union-Find의 find: x가 속한 집합의 대표 정점을 찾는다.
 * 찾는 김에 경로에 있는 정점들이 대표를 바로 가리키게 바꿔서(경로 압축) 다음 탐색을 빠르게 한다. */
static int findSet(int parent[], int x)
{
    if (parent[x] != x)
        parent[x] = findSet(parent, parent[x]);
    return parent[x];
}

int kruskal(listGraph* g, edge mst[], int* totalCost)
{
    int parent[VERTEX_COUNT];                          /* Union-Find: parent[x]는 x의 부모 정점 */
    edge* edges = (edge*)malloc(sizeof(edge) * g->e);  /* 그래프의 모든 간선을 모아 둘 배열 */
    int count = 0;                                     /* edges[]에 모은 간선 수 */
    int mstCount = 0;                                  /* MST에 넣은 간선 수 */
    int i, u, rootU, rootV;
    listNode* p;

    /* 1) 인접 리스트를 돌면서 모든 간선을 배열로 모은다.
     *    무방향 그래프는 간선 하나가 양쪽 리스트에 두 번 들어 있으므로 u < vertex 인 경우만 담는다. */
    for (u = 0; u < g->n; u++) {
        for (p = g->adjList_H[u]; p != NULL; p = p->link) {
            if (u < p->vertex) {
                edges[count].u = u;
                edges[count].v = p->vertex;
                edges[count].w = p->weight;
                count++;
            }
        }
    }

    /* 2) 가중치가 작은 순서로 정렬 */
    qsort(edges, count, sizeof(edge), compareEdge);

    /* 3) Union-Find 초기화: 처음에는 모든 정점이 각자 하나의 집합 */
    for (i = 0; i < g->n; i++)
        parent[i] = i;

    /* 4) 가중치가 작은 간선부터 확인하면서, 사이클이 생기지 않으면 MST에 추가 */
    *totalCost = 0;
    for (i = 0; i < count && mstCount < g->n - 1; i++) {
        rootU = findSet(parent, edges[i].u);
        rootV = findSet(parent, edges[i].v);

        if (rootU != rootV) {          /* 서로 다른 집합이면 사이클이 생기지 않는다 */
            parent[rootU] = rootV;     /* 두 집합을 하나로 합친다 (union) */
            mst[mstCount] = edges[i];
            mstCount++;
            *totalCost += edges[i].w;
        }
        /* rootU == rootV 이면 이미 연결된 두 정점이라 추가하면 사이클이 되므로 건너뛴다 */
    }

    free(edges);
    return mstCount;
}

/* ==================================================================
 * Prim 알고리즘 (인접 행렬)
 * ================================================================== */

int prim(matrixGraph* g, edge mst[], int* totalCost)
{
    int visited[VERTEX_COUNT];   /* 1이면 이미 MST에 포함된 정점 */
    int dist[VERTEX_COUNT];      /* dist[j]: MST와 정점 j를 연결하는 간선 중 가장 작은 가중치 */
    int from[VERTEX_COUNT];      /* from[j]: 그 가장 작은 간선의 MST쪽 끝 정점 */
    int i, j, u, minDist;
    int mstCount = 0;

    *totalCost = 0;
    for (i = 0; i < g->n; i++) {
        visited[i] = 0;
        dist[i] = INF;
        from[i] = -1;
    }
    dist[0] = 0;   /* 시작 정점은 0번 */

    for (i = 0; i < g->n; i++) {
        /* 1) MST에 포함되지 않은 정점 중 dist가 가장 작은 정점 u를 고른다 */
        u = -1;
        minDist = INF;
        for (j = 0; j < g->n; j++) {
            if (visited[j] == 0 && dist[j] < minDist) {
                minDist = dist[j];
                u = j;
            }
        }
        if (u == -1) break;   /* 더 연결할 정점이 없음 (연결 그래프에서는 일어나지 않는다) */

        /* 2) u를 MST에 추가. 시작 정점(from이 -1)은 간선이 없으므로 제외 */
        visited[u] = 1;
        if (from[u] != -1) {
            mst[mstCount].u = from[u];
            mst[mstCount].v = u;
            mst[mstCount].w = dist[u];
            mstCount++;
            *totalCost += dist[u];
        }

        /* 3) u가 새로 들어왔으니, u를 거쳐 가면 더 싸게 연결되는 정점의 dist를 갱신 */
        for (j = 0; j < g->n; j++) {
            if (visited[j] == 0 && g->adjMatrix[u][j] < dist[j]) {
                dist[j] = g->adjMatrix[u][j];
                from[j] = u;
            }
        }
    }
    return mstCount;
}

/* ==================================================================
 * 메모리 사용량
 * 그래프 표현 + 알고리즘의 주요 보조 자료구조 + MST 결과 배열(정점 수 - 1개 간선)
 * (난수 생성용 임시 배열과 malloc 내부 관리 영역은 제외)
 * ================================================================== */

 /* 인접 리스트 + Kruskal:
  *   리스트 구조체(헤드 포인터 배열 포함) + 노드 (간선 수 x 2개)
  *   + 정렬할 간선 배열(간선 수 x edge) + Union-Find의 parent 배열 + MST 결과 배열 */
int kruskalMemory(listGraph* g)
{
    int total = 0;
    total += (int)sizeof(listGraph);
    total += g->e * 2 * (int)sizeof(listNode);
    total += g->e * (int)sizeof(edge);
    total += VERTEX_COUNT * (int)sizeof(int);
    total += (VERTEX_COUNT - 1) * (int)sizeof(edge);
    return total;
}

/* 인접 행렬 + Prim:
 *   행렬 구조체 + visited, dist, from 배열(정점 수 x 3) + MST 결과 배열 */
int primMemory(matrixGraph* g)
{
    int total = 0;
    total += (int)sizeof(*g);
    total += 3 * VERTEX_COUNT * (int)sizeof(int);
    total += (VERTEX_COUNT - 1) * (int)sizeof(edge);
    return total;
}

/* ==================================================================
 * 출력
 * ================================================================== */

 /* MST 간선을 (u-v:가중치) 형태로 한 줄에 5개씩 출력 */
void printMST(edge mst[], int count)
{
    int i;
    printf("MST Edge List (u-v:weight)");
    for (i = 0; i < count; i++) {
        if (i % 5 == 0) printf("\n    ");
        printf("(%2d-%2d:%3d) ", mst[i].u, mst[i].v, mst[i].w);
    }
    printf("\n");
}