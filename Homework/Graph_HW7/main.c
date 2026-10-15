#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "graph.h"

/* 그래프 한 종류(희소 또는 밀집)에 대해
 * 인접 행렬 케이스와 인접 리스트 케이스를 차례로 실행하고 결과를 출력한다.
 * 두 케이스는 같은 간선 목록(edges)과 같은 정점 쌍을 사용한다. */
static void testGraph(const char* graphName, int edgeCount, int firstCaseNo)
{
    int edges[DENSE_EDGES][2];   /* 생성된 간선 목록 (두 표현에 똑같이 사용) */
    matrixGraph mg;              /* 인접 행렬 그래프 */
    listGraph lg;                /* 인접 리스트 그래프 */

    int insU, insV;   /* 삽입할 간선 (그래프에 없는 정점 쌍) */
    int delU, delV;   /* 삭제할 간선 (그래프에 있는 간선) */
    int conU, conV;   /* 연결 여부를 확인할 정점 쌍 */
    int adjU;         /* 인접 정점을 출력할 정점 */
    int i, result, adjCount;

    /* 1) 그래프 생성: 같은 간선 목록으로 행렬과 리스트를 모두 만든다 */
    createEdges(edges, edgeCount);
    createMatrixGraph(&mg);
    createListGraph(&lg);
    for (i = 0; i < edgeCount; i++) {
        insertMatrixEdge(&mg, edges[i][0], edges[i][1]);
        insertListEdge(&lg, edges[i][0], edges[i][1]);
    }

    /* 2) 테스트에 사용할 정점 쌍 선택 (행렬/리스트에 똑같이 적용) */
    do {   /* 삽입할 간선: 서로 다르고 아직 연결되지 않은 두 정점 */
        insU = rand() % MAX_VERTEX;
        insV = rand() % MAX_VERTEX;
    } while (insU == insV || mg.adjMatrix[insU][insV] == 1);

    i = rand() % edgeCount;   /* 삭제할 간선: 간선 목록에서 고르면 반드시 존재하는 간선 */
    delU = edges[i][0];
    delV = edges[i][1];

    do {   /* 연결 확인용: 서로 다른 임의의 두 정점 */
        conU = rand() % MAX_VERTEX;
        conV = rand() % MAX_VERTEX;
    } while (conU == conV);

    adjU = rand() % MAX_VERTEX;   /* 인접 정점을 출력할 정점 */

    printf("======== %s (Vertices: %d, Edges: %d) ========\n",
        graphName, MAX_VERTEX, edgeCount);
    printf("Insert edge  : (%d, %d)\n", insU, insV);
    printf("Delete edge  : (%d, %d)\n", delU, delV);
    printf("Connect pair : (%d, %d)\n", conU, conV);
    printf("Adjacent of  : %d\n\n", adjU);

    /* 3) 인접 행렬 케이스: 연산마다 compareCount를 0으로 만들고, 연산 후 값을 읽는다 */
    printf("Case %d : %s - Adjacency Matrix\n", firstCaseNo, graphName);
    printf("Memory                         : %d Bytes\n", matrixMemory(&mg));

    compareCount = 0;
    insertMatrixEdge(&mg, insU, insV);
    printf("Edge insertion comparisons     : %d\n", compareCount);

    compareCount = 0;
    deleteMatrixEdge(&mg, delU, delV);
    printf("Edge deletion comparisons      : %d\n", compareCount);

    compareCount = 0;
    result = existsMatrixEdge(&mg, conU, conV);
    printf("Connectivity check comparisons : %d (%s)\n", compareCount,
        result ? "connected" : "not connected");

    printf("Adjacent vertices of %d        :", adjU);
    compareCount = 0;
    adjCount = printMatrixAdjacent(&mg, adjU);
    printf("Adjacent vertices comparisons  : %d (%d adjacent vertices)\n\n",
        compareCount, adjCount);

    /* 4) 인접 리스트 케이스 */
    printf("Case %d : %s - Adjacency List\n", firstCaseNo + 1, graphName);
    printf("Memory                         : %d Bytes\n", listMemory(&lg));

    compareCount = 0;
    insertListEdge(&lg, insU, insV);
    printf("Edge insertion comparisons     : %d\n", compareCount);

    compareCount = 0;
    deleteListEdge(&lg, delU, delV);
    printf("Edge deletion comparisons      : %d\n", compareCount);

    compareCount = 0;
    result = existsListEdge(&lg, conU, conV);
    printf("Connectivity check comparisons : %d (%s)\n", compareCount,
        result ? "connected" : "not connected");

    printf("Adjacent vertices of %d        :", adjU);
    compareCount = 0;
    adjCount = printListAdjacent(&lg, adjU);
    printf("Adjacent vertices comparisons  : %d (%d adjacent vertices)\n\n",
        compareCount, adjCount);

    /* 5) 인접 리스트의 노드 메모리 해제 */
    freeListGraph(&lg);
}

int main(void)
{
    /* 실행할 때마다 다른 랜덤 그래프가 나오도록 시드 설정 */
    srand((unsigned int)time(NULL));

    testGraph("Sparse Graph", SPARSE_EDGES, 1);   /* Case 1, 2 */
    testGraph("Dense Graph", DENSE_EDGES, 3);     /* Case 3, 4 */

    return 0;
}