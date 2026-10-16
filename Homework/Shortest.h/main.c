#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "shortest.h"

int main(void)
{
    edge edges[EDGE_COUNT];                  /* 랜덤으로 만든 간선 목록 */
    matrixGraph g;                           /* 가중치 인접 행렬 그래프 */
    int dist[VERTEX_COUNT][VERTEX_COUNT];    /* dist[s][v]: 시작 정점 s에서 v까지의 최단거리 */
    int prev[VERTEX_COUNT][VERTEX_COUNT];    /* prev[s][v]: 시작 정점 s에서 v까지 가는 최단경로에서 v의 바로 앞 정점 */
    int i, s;

    /* 실행할 때마다 다른 랜덤 그래프가 나오도록 시드 설정 */
    srand((unsigned int)time(NULL));

    /* 1) 그래프 생성: 연결 그래프인지 확인하고, 연결되지 않았다면 다시 만든다. 
     *    (createEdges가 연결을 보장하지만 과제 조건에 따라 한 번 더 확인한다) */
    do {
        createEdges(edges, EDGE_COUNT);
        createGraph(&g);
        for (i = 0; i < EDGE_COUNT; i++)
            insertEdge(&g, edges[i].u, edges[i].v, edges[i].w);
    } while (!isConnected(&g));

    /* 2) 생성된 그래프 출력 */
    printGraph(&g);

    /* 3) 모든 정점 쌍의 최단경로 계산:
     *    각 정점을 시작 정점으로 해서 Dijkstra 알고리즘을 정점 수(10)번 반복 수행한다 */
    for (s = 0; s < g.n; s++)
        dijkstra(&g, s, dist[s], prev[s]);

    /* 4) 45개 정점 쌍의 최단경로와 최단거리 출력 */
    printShortestPaths(&g, dist, prev);

    /* 5) 확인 사항 검사 결과 출력 */
    printVerification(&g, dist, prev);

    return 0;
}