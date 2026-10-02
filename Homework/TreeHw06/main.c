#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "AVL.h"

#define MAX_VALUE 1000
#define DATA_COUNT 100
#define KEY_COUNT 50

int main(void) {
    srand((unsigned)time(NULL));

    /* 생성된 100개의 원본 데이터 */
    int generated[DATA_COUNT];

    /* 중복 제거 후 배열에 저장되는 데이터 */
    int arr[DATA_COUNT];
    int arrSize = 0;

    /* 검색할 50개의 키 */
    int keys[KEY_COUNT];

    /* 트리 */
    BSTNode* bstRoot = NULL;
    AVLNode* avlRoot = NULL;

    /* 생성 과정의 비교 횟수 */
    int arrBuildCount = 0;
    int bstBuildCount = 0;
    int avlBuildCount = 0;

    /* 중복 개수 */
    int duplicateCount = 0;


    /* =========================================
       1. 100개의 랜덤 정수 생성
       ========================================= */

    for (int i = 0; i < DATA_COUNT; i++) {

        int data = rand() % (MAX_VALUE + 1);

        generated[i] = data;


        /* -------------------------
           배열 중복 검사
           ------------------------- */

        int found = 0;

        int count = sequentialSearch(
            arr,
            arrSize,
            data,
            &found
        );

        arrBuildCount += count;

        if (found) {
            duplicateCount++;
        }
        else {
            arr[arrSize] = data;
            arrSize++;
        }


        /* -------------------------
           BST 삽입
           ------------------------- */

        bstRoot = insertBST(
            bstRoot,
            data,
            &bstBuildCount
        );


        /* -------------------------
           AVL 삽입
           ------------------------- */

        avlRoot = insertAVL(
            avlRoot,
            data,
            &avlBuildCount
        );
    }


    /* =========================================
       2. 생성된 100개의 정수 출력
       ========================================= */

    printf("=== 생성된 100개의 정수 ===\n");

    for (int i = 0; i < DATA_COUNT; i++) {

        printf("%4d", generated[i]);

        if (i % 10 == 9) {
            printf("\n");
        }
    }


    /* =========================================
       3. 생성 결과 출력
       ========================================= */

    printf("\n");

    printf("실제 저장된 서로 다른 정수 개수: %d\n", arrSize);
    printf("중복 개수: %d\n", duplicateCount);

    printf("\n=== 생성 비교 횟수 ===\n");

    printf("배열: %d회\n", arrBuildCount);
    printf("BST : %d회\n", bstBuildCount);
    printf("AVL : %d회\n", avlBuildCount);

    printf("\n=== 자료구조 정보 ===\n");

    printf("배열 길이: %d\n", arrSize);
    printf("BST 높이: %d\n", getHeightBST(bstRoot));
    printf("AVL 높이: %d\n", getHeightAVL(avlRoot));


    /* =========================================
       4. 검색할 50개의 랜덤 키 생성
       ========================================= */

    for (int i = 0; i < KEY_COUNT; i++) {
        keys[i] = rand() % (MAX_VALUE + 1);
    }


    /* 검색 비교 횟수 총합 */
    int seqTotal = 0;
    int bstTotal = 0;
    int avlTotal = 0;


    /* =========================================
       5. 50개 키 검색
       ========================================= */

    printf("\n=== 검색 결과 ===\n");

    printf(
        "%-4s %-6s %-10s %-8s %-8s %-8s\n",
        "No",
        "Key",
        "Result",
        "Seq",
        "BST",
        "AVL"
    );


    for (int i = 0; i < KEY_COUNT; i++) {

        int seqFound;
        int bstFound;
        int avlFound;


        /* -------------------------
           순차 탐색
           ------------------------- */

        int seqCount = sequentialSearch(
            arr,
            arrSize,
            keys[i],
            &seqFound
        );


        /* -------------------------
           BST 탐색
           ------------------------- */

        int bstCount = searchBST(
            bstRoot,
            keys[i],
            &bstFound
        );


        /* -------------------------
           AVL 탐색
           ------------------------- */

        int avlCount = searchAVL(
            avlRoot,
            keys[i],
            &avlFound
        );


        /* 검색 결과 */

        char* result;

        if (seqFound) {
            result = "Found";
        }
        else {
            result = "NOT Found";
        }


        /* 한 줄 출력 */

        printf(
            "%-4d %-6d %-10s %-8d %-8d %-8d\n",
            i + 1,
            keys[i],
            result,
            seqCount,
            bstCount,
            avlCount
        );


        /* 총합 */

        seqTotal += seqCount;
        bstTotal += bstCount;
        avlTotal += avlCount;
    }


    /* =========================================
       6. 검색 통계
       ========================================= */

    printf("\n=== 검색 통계 ===\n");

    printf(
        "[순차 탐색] 총 %d회, 평균 %.2f회\n",
        seqTotal,
        (double)seqTotal / KEY_COUNT
    );

    printf(
        "[BST 탐색]  총 %d회, 평균 %.2f회\n",
        bstTotal,
        (double)bstTotal / KEY_COUNT
    );

    printf(
        "[AVL 탐색]  총 %d회, 평균 %.2f회\n",
        avlTotal,
        (double)avlTotal / KEY_COUNT
    );


    /* =========================================
       7. 생성 + 탐색 비교 횟수
       ========================================= */

    printf("\n=== 생성 + 탐색 비교 횟수 ===\n");

    printf(
        "[BST] 총 %d회\n",
        bstBuildCount + bstTotal
    );

    printf(
        "[AVL] 총 %d회\n",
        avlBuildCount + avlTotal
    );


    /* =========================================
       8. 동적 메모리 해제
       ========================================= */

    destroyBST(bstRoot);
    destroyAVL(avlRoot);


    return 0;
}