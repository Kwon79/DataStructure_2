#include <stdio.h>
#include <stdlib.h>
#include "graph.h"

/* 비교 횟수 카운터.
 * 행렬의 칸이나 리스트의 노드를 "한 번 확인할 때마다" 1씩 증가시킨다. */
int compareCount = 0;

/* ==================================================================
 * 랜덤 간선 생성
 * ================================================================== */

void createEdges(int edges[][2], int edgeCount)
{
    int used[MAX_VERTEX][MAX_VERTEX] = { 0 };  /* 이미 만든 간선 표시용 임시 배열 (메모리 측정 대상 아님) */
    int made = 0;                              /* 지금까지 만든 간선 수 */
    int u, v;

    while (made < edgeCount) {
        u = rand() % MAX_VERTEX;
        v = rand() % MAX_VERTEX;

        if (u == v) continue;           /* 자기 자신과 연결 금지 */
        if (used[u][v] == 1) continue;  /* 중복 간선 금지 */

        used[u][v] = 1;
        used[v][u] = 1;                 /* 무방향이므로 양쪽 모두 표시 */
        edges[made][0] = u;
        edges[made][1] = v;
        made++;
    }
}

/* ==================================================================
 * 인접 행렬
 * ================================================================== */

 /* 정점 100개, 간선이 하나도 없는 인접 행렬을 만든다 */
void createMatrixGraph(matrixGraph* g)
{
    int i, j;
    g->n = MAX_VERTEX;
    for (i = 0; i < MAX_VERTEX; i++)
        for (j = 0; j < MAX_VERTEX; j++)
            g->adjMatrix[i][j] = 0;
}

/* 연결 여부 확인: adjMatrix[u][v] 한 칸만 보면 되므로 항상 비교 1회 */
int existsMatrixEdge(matrixGraph* g, int u, int v)
{
    compareCount++;
    return g->adjMatrix[u][v] == 1;
}

/* 간선 삽입: 이미 있는지 확인(비교 1회) -> 없으면 두 칸을 1로 바꾼다.
 * 값을 바꾸는 것은 "확인"이 아니므로 비교 횟수에 넣지 않는다. */
void insertMatrixEdge(matrixGraph* g, int u, int v)
{
    if (existsMatrixEdge(g, u, v)) return;
    g->adjMatrix[u][v] = 1;
    g->adjMatrix[v][u] = 1;   /* 무방향 그래프이므로 대칭으로 저장 */
}

/* 간선 삭제: 있는지 확인(비교 1회) -> 있으면 두 칸을 0으로 바꾼다 */
void deleteMatrixEdge(matrixGraph* g, int u, int v)
{
    if (!existsMatrixEdge(g, u, v)) return;
    g->adjMatrix[u][v] = 0;
    g->adjMatrix[v][u] = 0;
}

/* 인접 정점 출력: u번 행의 100칸을 모두 확인해야 하므로 항상 비교 100회 */
int printMatrixAdjacent(matrixGraph* g, int u)
{
    int v;
    int count = 0;   /* 출력한 인접 정점 수 */

    for (v = 0; v < g->n; v++) {
        compareCount++;
        if (g->adjMatrix[u][v] == 1) {
            if (count % 20 == 0) printf("\n    ");   /* 20개마다 줄바꿈 */
            printf("%3d", v);
            count++;
        }
    }
    printf("\n");
    return count;
}

/* 인접 행렬이 사용하는 메모리: 구조체 전체 (행렬 + 정점 수 n) */
int matrixMemory(matrixGraph* g)
{
    return (int)sizeof(*g);
}

/* ==================================================================
 * 인접 리스트
 * ================================================================== */

 /* 정점 100개, 간선이 하나도 없는 인접 리스트를 만든다 (헤드 포인터는 모두 NULL) */
void createListGraph(listGraph* g)
{
    int i;
    g->n = MAX_VERTEX;
    for (i = 0; i < MAX_VERTEX; i++)
        g->adjList_H[i] = NULL;
}

/* 인접 리스트에 할당한 노드를 모두 free */
void freeListGraph(listGraph* g)
{
    int i;
    graphNode* p;
    graphNode* next;

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

/* 연결 여부 확인: u의 인접 리스트를 앞에서부터 확인 (v를 찾으면 바로 중단).
 * 노드를 하나 확인할 때마다 비교 1회 */
int existsListEdge(listGraph* g, int u, int v)
{
    graphNode* p = g->adjList_H[u];

    while (p != NULL) {
        compareCount++;
        if (p->vertex == v) return 1;
        p = p->link;
    }
    return 0;
}

/* u의 인접 리스트 맨 앞에 v 노드를 추가한다 (맨 앞에 추가하므로 비교가 필요 없다) */
static void addFirstNode(listGraph* g, int u, int v)
{
    graphNode* newNode = (graphNode*)malloc(sizeof(graphNode));
    newNode->vertex = v;
    newNode->link = g->adjList_H[u];   /* 새 노드가 기존 첫 노드를 가리키게 한다 */
    g->adjList_H[u] = newNode;         /* 새 노드를 첫 노드로 만든다 */
}

/* 간선 삽입: 이미 있는지 u의 리스트를 확인(비교 발생) -> 없으면 양쪽 리스트에 추가 */
void insertListEdge(listGraph* g, int u, int v)
{
    if (existsListEdge(g, u, v)) return;
    addFirstNode(g, u, v);
    addFirstNode(g, v, u);   /* 무방향 그래프이므로 반대쪽 리스트에도 저장 */
}

/* head로 시작하는 리스트에서 v 노드를 찾아 지우고, 바뀐 head를 돌려준다.
 * 노드를 하나 확인할 때마다 비교 1회 */
static graphNode* removeNode(graphNode* head, int v)
{
    graphNode* prev = NULL;   /* 현재 노드의 바로 앞 노드 */
    graphNode* p = head;      /* 현재 확인 중인 노드 */

    while (p != NULL) {
        compareCount++;
        if (p->vertex == v) {
            if (prev == NULL) head = p->link;   /* 첫 노드를 지우는 경우: head가 바뀐다 */
            else prev->link = p->link;          /* 앞 노드가 다음 노드를 가리키게 연결 */
            free(p);
            return head;
        }
        prev = p;
        p = p->link;
    }
    return head;   /* 못 찾았으면 그대로 */
}

/* 간선 삭제: u의 리스트에서 v를, v의 리스트에서 u를 각각 찾아 지운다.
 * 리스트를 두 번 탐색하므로 두 탐색에서 확인한 노드 수를 모두 센다. */
void deleteListEdge(listGraph* g, int u, int v)
{
    g->adjList_H[u] = removeNode(g->adjList_H[u], v);
    g->adjList_H[v] = removeNode(g->adjList_H[v], u);
}

/* 인접 정점 출력: u의 리스트에 있는 노드만 확인 (비교 횟수 = 인접 정점 수) */
int printListAdjacent(listGraph* g, int u)
{
    graphNode* p = g->adjList_H[u];
    int count = 0;   /* 출력한 인접 정점 수 */

    while (p != NULL) {
        compareCount++;
        if (count % 20 == 0) printf("\n    ");   /* 20개마다 줄바꿈 */
        printf("%3d", p->vertex);
        count++;
        p = p->link;
    }
    printf("\n");
    return count;
}

/* 인접 리스트가 사용하는 메모리: 구조체(헤드 포인터 배열 + n) + 현재 노드 수 x 노드 크기 */
int listMemory(listGraph* g)
{
    int i;
    int nodeCount = 0;
    graphNode* p;

    for (i = 0; i < g->n; i++) {
        p = g->adjList_H[i];
        while (p != NULL) {
            nodeCount++;
            p = p->link;
        }
    }
    return (int)sizeof(*g) + nodeCount * (int)sizeof(graphNode);
}