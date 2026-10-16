#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mst.h"

/* 같은 간선 목록으로 인접 리스트와 인접 행렬을 모두 만든다.
 * 그래프의 정점, 간선, 가중치가 두 표현에서 완전히 똑같다. */
static void buildGraph(int edgeCount, listGraph* lg, matrixGraph* mg)
{
    edge edges[DENSE_EDGES];   /* 랜덤으로 만든 간선 목록 (임시 배열) */
    int i;

    /* 연결 그래프인지 확인하고, 연결되지 않았다면 다시 만든다.
     * (createEdges가 연결을 보장하지만 과제 조건에 따라 한 번 더 확인한다) */
    do {
        createEdges(edges, edgeCount);
        createMatrixGraph(mg);
        for (i = 0; i < edgeCount; i++)
            insertMatrixEdge(mg, edges[i].u, edges[i].v, edges[i].w);
    } while (!isConnected(mg));

    createListGraph(lg);
    for (i = 0; i < edgeCount; i++)
        insertListEdge(lg, edges[i].u, edges[i].v, edges[i].w);
}

/* 인접 리스트 + Kruskal 케이스를 실행하고 결과를 출력한다. MST 전체 비용을 반환 */
static int runKruskal(int caseNo, const char* title, listGraph* g)
{
    edge mst[VERTEX_COUNT - 1];   /* MST 간선 저장 (정점 수 - 1개) */
    int cost = 0, count = 0, r;
    clock_t start, end;
    double ms;

    /* 실행시간 측정: MST 한 번은 clock()의 정밀도(약 1ms)보다 훨씬 짧게 끝나서
     * REPEAT번 반복 실행한 전체 시간을 재고 1번당 평균 시간을 구한다.
     * (그래프 생성과 출력은 측정 구간에 포함되지 않는다) */
    start = clock();
    for (r = 0; r < REPEAT; r++)
        count = kruskal(g, mst, &cost);
    end = clock();
    ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC / REPEAT;

    printf("Case %d : %s\n", caseNo, title);
    printf("Vertices      : %d\n", g->n);
    printf("Edges         : %d\n", g->e);
    printf("MST Edges     : %d\n", count);
    printf("MST Cost      : %d\n", cost);
    printf("Memory        : %d Bytes\n", kruskalMemory(g));
    printf("Execution Time: %.4f ms\n", ms);
    printMST(mst, count);
    printf("\n");
    return cost;
}

/* 인접 행렬 + Prim 케이스를 실행하고 결과를 출력한다. MST 전체 비용을 반환 */
static int runPrim(int caseNo, const char* title, matrixGraph* g)
{
    edge mst[VERTEX_COUNT - 1];
    int cost = 0, count = 0, r;
    clock_t start, end;
    double ms;

    start = clock();
    for (r = 0; r < REPEAT; r++)
        count = prim(g, mst, &cost);
    end = clock();
    ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC / REPEAT;

    printf("Case %d : %s\n", caseNo, title);
    printf("Vertices      : %d\n", g->n);
    printf("Edges         : %d\n", g->e);
    printf("MST Edges     : %d\n", count);
    printf("MST Cost      : %d\n", cost);
    printf("Memory        : %d Bytes\n", primMemory(g));
    printf("Execution Time: %.4f ms\n", ms);
    printMST(mst, count);
    printf("\n");
    return cost;
}

int main(void)
{
    listGraph sparseList, denseList;           /* 인접 리스트 (Kruskal용) */
    matrixGraph sparseMatrix, denseMatrix;     /* 인접 행렬 (Prim용) */
    int kruskalSparse, kruskalDense, primSparse, primDense;

    /* 실행할 때마다 다른 랜덤 그래프가 나오도록 시드 설정 */
    srand((unsigned int)time(NULL));

    /* 희소/밀집 그래프를 각각 한 번씩만 만들고, 같은 그래프를 두 알고리즘이 공유한다 */
    buildGraph(SPARSE_EDGES, &sparseList, &sparseMatrix);
    buildGraph(DENSE_EDGES, &denseList, &denseMatrix);

    kruskalSparse = runKruskal(1, "Sparse Graph - Adjacency List - Kruskal", &sparseList);
    kruskalDense = runKruskal(2, "Dense Graph - Adjacency List - Kruskal", &denseList);
    primSparse = runPrim(3, "Sparse Graph - Adjacency Matrix - Prim", &sparseMatrix);
    primDense = runPrim(4, "Dense Graph - Adjacency Matrix - Prim", &denseMatrix);

    /* 같은 그래프라면 MST 전체 비용은 두 알고리즘에서 같아야 한다 */
    printf("MST Cost Check\n");
    printf("Sparse Graph : Kruskal %d, Prim %d -> %s\n", kruskalSparse, primSparse,
        kruskalSparse == primSparse ? "same" : "DIFFERENT");
    printf("Dense Graph  : Kruskal %d, Prim %d -> %s\n", kruskalDense, primDense,
        kruskalDense == primDense ? "same" : "DIFFERENT");

    freeListGraph(&sparseList);
    freeListGraph(&denseList);
    return 0;
}