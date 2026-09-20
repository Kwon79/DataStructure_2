#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

#include "BinaryTreeArray.h"


int main()
{
    ArrayTree tree;

    char input[MAX];
    char target;


    // 트리 초기화
    initTree(&tree);


    // 트리 입력
    printf("이진트리를 입력하세요: ");
    scanf("%99s", input);


    // 트리 생성
    if (!buildTree(&tree, input))
    {
        printf("Error: 잘못된 이진트리 형식입니다.\n");
        return 1;
    }


    // 트리 출력
    printf("\n===== 이진트리 =====\n");
    printTree(&tree);


    // 트리 정보
    printTreeInfo(&tree);


    // 이진트리 형태
    printTreeType(&tree);


    // 특정 노드 입력
    printf("\n특정 노드를 입력하세요: ");
    scanf(" %c", &target);


    // 부모
    printParent(&tree, target);

    // 자식
    printChildren(&tree, target);

    // 형제
    printSibling(&tree, target);


    return 0;
}