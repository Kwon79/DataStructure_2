#pragma once
#ifndef SHORTEST_H
#define SHORTEST_H

/* 과제 09: 모든 정점 쌍의 최단경로 탐색 (헤더 파일) */

#define VERTEX_COUNT 10    /* 정점 수 (번호 0 ~ 9) */
#define EDGE_COUNT   20    /* 간선 수 */
#define MAX_WEIGHT   100   /* 간선 가중치는 1 ~ MAX_WEIGHT 사이의 양의 정수 */
#define INF          1000000  /* 간선이 없거나 갈 수 없는 경우를 나타내는 값 (무한대 대신 사용) */

/* 간선 하나: 정점 u, v와 가중치 w */
typedef struct {
    int u;
    int v;
    int w;
} edge;

/* 가중치 인접 행렬 그래프 */
typedef struct {
    int n;                                        /* 정점 개수 */
    int weight[VERTEX_COUNT][VERTEX_COUNT];       /* weight[i][j]: i-j 간선의 가중치
                                                     (간선이 없으면 INF, 자기 자신(대각선)은 0) */
} matrixGraph;

/* ---------- 랜덤 그래프 생성 ---------- */
/* 연결 그래프가 되도록 중복/자기 자신 연결이 없는 랜덤 가중치 간선 edgeCount개를 만든다 */
void createEdges(edge edges[], int edgeCount);

/* ---------- 그래프 ---------- */
void createGraph(matrixGraph* g);                      /* 간선이 없는 그래프 생성 */
void insertEdge(matrixGraph* g, int u, int v, int w);  /* 무방향 간선 삽입 */
int  isConnected(matrixGraph* g);                      /* 모든 정점이 연결되어 있으면 1, 아니면 0 */

/* ---------- 최단경로 ---------- */
/* Dijkstra 알고리즘: 시작 정점 start에서 다른 모든 정점까지의 최단거리를 구한다.
 *   dist[v]: start에서 v까지의 최단거리
 *   prev[v]: start에서 v까지 가는 최단경로에서 v 바로 앞에 있는 정점 (start는 -1) */
void dijkstra(matrixGraph* g, int start, int dist[], int prev[]);

/* prev를 거꾸로 따라가며 u에서 v까지의 최단경로를 path에 저장하고, 경로에 포함된 정점 수를 반환.
 * prev[u][v]는 "u를 시작 정점으로 했을 때 v의 바로 앞 정점"이다. */
int getPath(int prev[][VERTEX_COUNT], int u, int v, int path[]);

/* 경로를 따라 간선 가중치를 직접 더한 값을 반환 (경로에 없는 간선이 있으면 -1) */
int pathCost(matrixGraph* g, int path[], int count);

/* ---------- 출력 ---------- */
void printGraph(matrixGraph* g);
void printShortestPaths(matrixGraph* g, int dist[][VERTEX_COUNT], int prev[][VERTEX_COUNT]);
void printVerification(matrixGraph* g, int dist[][VERTEX_COUNT], int prev[][VERTEX_COUNT]);

#endif