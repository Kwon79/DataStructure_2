#include <stdio.h>
#include <stdlib.h>
#include "shortest.h"

/* ==================================================================
 * 랜덤 그래프 생성
 * ================================================================== */

void createEdges(edge edges[], int edgeCount)
{
    int used[VERTEX_COUNT][VERTEX_COUNT] = { 0 };  /* 이미 만든 간선 표시용 임시 배열 */
    int made = 0;                                  /* 지금까지 만든 간선 수 */
    int i, u, v;

    /* 1) 연결 그래프를 보장하기 위해 먼저 간선 9개짜리 트리를 만든다.
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
 * 가중치 인접 행렬 그래프
 * ================================================================== */

 /* 간선이 없는 그래프를 만든다: 자기 자신까지의 거리는 0, 나머지는 INF */
void createGraph(matrixGraph* g)
{
    int i, j;
    g->n = VERTEX_COUNT;
    for (i = 0; i < VERTEX_COUNT; i++)
        for (j = 0; j < VERTEX_COUNT; j++)
            g->weight[i][j] = (i == j) ? 0 : INF;
}

void insertEdge(matrixGraph* g, int u, int v, int w)
{
    g->weight[u][v] = w;
    g->weight[v][u] = w;   /* 무방향 그래프이므로 대칭으로 저장 */
}

/* 깊이 우선 탐색으로 정점 v에서 갈 수 있는 정점을 모두 방문 표시한다 */
static void dfs(matrixGraph* g, int v, int visited[])
{
    int w;
    visited[v] = 1;
    for (w = 0; w < g->n; w++)
        if (g->weight[v][w] != INF && visited[w] == 0)
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
 * Dijkstra 최단경로 알고리즘
 * ================================================================== */

 /* 시작 정점 start에서 다른 모든 정점까지의 최단거리(dist)와, 최단경로에서 각 정점의 바로 앞 정점(prev)을 구한다 */
void dijkstra(matrixGraph* g, int start, int dist[], int prev[])
{
    int found[VERTEX_COUNT];   /* found[v]가 1이면 start에서 v까지의 최단거리가 확정된 정점 (수업의 집합 S) */
    int i, j, u, minDist;

    /* 1) 초기화: start에서 간선으로 바로 갈 수 있는 거리를 dist에 넣는다.
     *    바로 갈 수 있는 정점은 바로 앞 정점이 start이고, 갈 수 없는 정점은 -1(아직 경로 없음) */
    for (i = 0; i < g->n; i++) {
        found[i] = 0;
        dist[i] = g->weight[start][i];
        if (g->weight[start][i] != INF)
            prev[i] = start;
        else
            prev[i] = -1;
    }
    found[start] = 1;    /* start 자신은 거리 0으로 이미 확정 */
    dist[start] = 0;
    prev[start] = -1;    /* start는 출발점이라 앞 정점이 없다 */

    /* 2) 아직 확정되지 않은 정점 중 dist가 가장 작은 정점 u를 하나씩 확정하는 과정을 반복한다 */
    for (i = 0; i < g->n - 1; i++) {
        u = -1;
        minDist = INF;
        for (j = 0; j < g->n; j++) {
            if (found[j] == 0 && dist[j] < minDist) {
                minDist = dist[j];
                u = j;
            }
        }
        if (u == -1) break;   /* 더 갈 수 있는 정점이 없음 (연결 그래프에서는 일어나지 않는다) */
        found[u] = 1;         /* u의 최단거리 확정 */

        /* 3) u를 거쳐 가면 더 짧아지는 정점이 있는지 확인해서 dist와 prev를 갱신한다 */
        for (j = 0; j < g->n; j++) {
            if (found[j] == 0 && g->weight[u][j] != INF &&
                dist[u] + g->weight[u][j] < dist[j]) {
                dist[j] = dist[u] + g->weight[u][j];
                prev[j] = u;   /* j 바로 앞 정점은 u */
            }
        }
    }
}

/* u에서 v까지의 최단경로를 복원한다.
 * prev는 "바로 앞 정점"이라서 v에서부터 거꾸로 따라가며 모은 뒤, 순서를 뒤집어서 u -> ... -> v 로 만든다.
 * 예: prev[0][3] = 8, prev[0][8] = 1, prev[0][1] = 0 이면 3 <- 8 <- 1 <- 0 을 뒤집어 0 -> 1 -> 8 -> 3 */
int getPath(int prev[][VERTEX_COUNT], int u, int v, int path[])
{
    int temp[VERTEX_COUNT];   /* v에서 u 방향으로 거꾸로 모은 정점들 */
    int count = 0;
    int cur = v;
    int i;

    while (cur != u) {
        if (cur == -1 || count >= VERTEX_COUNT - 1) return 0;   /* 경로가 없거나 비정상 (무한 반복 방지) */
        temp[count] = cur;
        count++;
        cur = prev[u][cur];   /* u를 시작 정점으로 했을 때 cur의 바로 앞 정점으로 이동 */
    }
    temp[count] = u;
    count++;

    for (i = 0; i < count; i++)
        path[i] = temp[count - 1 - i];   /* 거꾸로 모은 것을 뒤집어서 저장 */
    return count;
}

/* 경로를 따라 간선 가중치를 하나씩 더해서 총 비용을 구한다 (검증용) */
int pathCost(matrixGraph* g, int path[], int count)
{
    int i, cost = 0;

    for (i = 0; i + 1 < count; i++) {
        if (g->weight[path[i]][path[i + 1]] == INF) return -1;   /* 간선이 없는 구간이 있음 */
        cost += g->weight[path[i]][path[i + 1]];
    }
    return cost;
}

/* ==================================================================
 * 출력
 * ================================================================== */

 /* 행렬을 u < v 순서로 훑으면서 간선과 가중치를 출력한다 */
void printGraph(matrixGraph* g)
{
    int u, v, count = 0;

    for (u = 0; u < g->n; u++)
        for (v = u + 1; v < g->n; v++)
            if (g->weight[u][v] != INF)
                count++;

    printf("Graph\n");
    printf("Vertices : %d\n", g->n);
    printf("Edges    : %d\n", count);
    printf("\nEdges\n");

    for (u = 0; u < g->n; u++)
        for (v = u + 1; v < g->n; v++)
            if (g->weight[u][v] != INF)
                printf("(%d, %d) Weight: %d\n", u, v, g->weight[u][v]);
    printf("\n");
}

/* u < v 인 모든 정점 쌍(45쌍)의 최단경로와 최단거리를 출력한다 */
void printShortestPaths(matrixGraph* g, int dist[][VERTEX_COUNT], int prev[][VERTEX_COUNT])
{
    int u, v, i, count;
    int pairs = 0;
    int path[VERTEX_COUNT];

    printf("Shortest Paths\n\n");
    for (u = 0; u < g->n; u++) {
        for (v = u + 1; v < g->n; v++) {
            count = getPath(prev, u, v, path);

            printf("%d -> %d\n", u, v);
            printf("Path     : ");
            for (i = 0; i < count; i++) {
                if (i > 0) printf(" -> ");
                printf("%d", path[i]);
            }
            printf("\n");
            printf("Distance : %d\n\n", dist[u][v]);
            pairs++;
        }
    }
    printf("Total vertex pairs : %d\n\n", pairs);
}

/* 과제의 "확인 사항"을 프로그램이 직접 검사해서 출력한다 */
void printVerification(matrixGraph* g, int dist[][VERTEX_COUNT], int prev[][VERTEX_COUNT])
{
    int u, v;
    int edgeCount = 0;        /* 행렬에서 센 간선 수 */
    int selfLoop = 0;         /* 자기 자신을 연결한 간선 수 */
    int weightOk = 1;         /* 모든 가중치가 1 ~ MAX_WEIGHT 범위인지 */
    int pairs = 0;            /* 검사한 정점 쌍 수 */
    int correct = 0;          /* 경로 비용 = 최단거리가 맞는 정점 쌍 수 */
    int path[VERTEX_COUNT];
    int count;

    for (u = 0; u < g->n; u++) {
        if (g->weight[u][u] != 0) selfLoop++;     /* 대각선이 0이 아니면 자기 자신 연결 */
        for (v = u + 1; v < g->n; v++) {
            if (g->weight[u][v] != INF) {
                edgeCount++;
                if (g->weight[u][v] < 1 || g->weight[u][v] > MAX_WEIGHT) weightOk = 0;
            }
        }
    }

    for (u = 0; u < g->n; u++) {
        for (v = u + 1; v < g->n; v++) {
            count = getPath(prev, u, v, path);
            pairs++;
            /* 출력된 경로의 간선 가중치를 직접 더한 값이 최단거리와 같은지, 경로가 u에서 시작해 v로 끝나는지 확인 */
            if (count > 0 && path[0] == u && path[count - 1] == v &&
                pathCost(g, path, count) == dist[u][v])
                correct++;
        }
    }

    printf("Verification\n");
    printf("Vertices                       : %d\n", g->n);
    printf("Edges (counted in matrix)      : %d\n", edgeCount);
    printf("Self-loops                     : %d\n", selfLoop);
    printf("Duplicate edges                : %s\n", edgeCount == EDGE_COUNT ? "none" : "FOUND");
    printf("Weights in range 1 - %d       : %s\n", MAX_WEIGHT, weightOk ? "yes" : "NO");
    printf("Connected graph                : %s\n", isConnected(g) ? "yes" : "NO");
    printf("Vertex pairs printed           : %d\n", pairs);
    printf("Path cost == Distance          : %d / %d pairs\n", correct, pairs);
}