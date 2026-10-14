#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "graph.h"

/* ------------------------------------------------------------------
 * 실행 파일: 희소/밀집 그래프를 생성하고 4가지 케이스를 측정/출력한다.
 * ------------------------------------------------------------------ */

 /* 한 종류의 그래프(희소 또는 밀집)에 대해 행렬/리스트 두 케이스를 실행.
  * 두 표현 방식은 같은 edges 배열과 같은 TestTargets를 사용한다. */
static void run_graph(const char* graphName, int edgeCount,
    const char* matrixCaseName, const char* listCaseName,
    Result* matrixOut, Result* listOut)
{
    Edge* edges = (Edge*)malloc(sizeof(Edge) * edgeCount); /* 임시 배열(측정 제외) */
    MatrixGraph* mg = (MatrixGraph*)malloc(sizeof(MatrixGraph));
    ListGraph lg;
    TestTargets t;
    Result mr, lr;

    if (edges == NULL || mg == NULL) {
        fprintf(stderr, "메모리 할당 실패\n");
        exit(1);
    }

    /* 1) 그래프 생성: 간선 목록을 한 번만 만들고 두 표현에 똑같이 적용 */
    generate_random_edges(edges, edgeCount);
    matrix_build(mg, edges, edgeCount);
    list_build(&lg, edges, edgeCount);

    /* 2) 두 표현에 공통으로 쓸 정점 쌍/정점 선택 */
    pick_test_targets(edges, edgeCount, &t);

    printf("======== %s (Vertices: %d, Edges: %d) ========\n",
        graphName, NUM_VERTICES, edgeCount);
    printf("Insert edge  : (%d, %d)\n", t.insertEdge.u, t.insertEdge.v);
    printf("Delete edge  : (%d, %d)\n", t.deleteEdge.u, t.deleteEdge.v);
    printf("Connect pair : (%d, %d)\n", t.connectPair.u, t.connectPair.v);
    printf("Adjacent of  : %d\n\n", t.adjacentVertex);

    /* 3) 측정 */
    measure_matrix(mg, &t, &mr);
    measure_list(&lg, &t, &lr);

    /* 4) 출력 */
    print_result(matrixCaseName, &mr);
    print_result(listCaseName, &lr);

    /* 요약표 출력을 위해 결과를 호출한 쪽으로 복사 */
    *matrixOut = mr;
    *listOut = lr;

    /* 5) 해제 */
    list_free(&lg);
    free(mg);
    free(edges);
}

int main(void)
{
    /* 실행할 때마다 다른 랜덤 그래프가 나오도록 시드 설정.
     * 같은 결과를 반복 재현하려면 srand(1); 처럼 고정값을 사용하면 된다. */
    Result results[4]; /* 0:희소-행렬 1:희소-리스트 2:밀집-행렬 3:밀집-리스트 */

    srand((unsigned int)time(NULL));

    run_graph("Sparse Graph", SPARSE_EDGES,
        "Case 1 : Sparse Graph - Adjacency Matrix",
        "Case 2 : Sparse Graph - Adjacency List",
        &results[0], &results[1]);

    run_graph("Dense Graph", DENSE_EDGES,
        "Case 3 : Dense Graph - Adjacency Matrix",
        "Case 4 : Dense Graph - Adjacency List",
        &results[2], &results[3]);

    print_summary(results);

    return 0;
}