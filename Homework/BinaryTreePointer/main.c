#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include "BinaryTreePointer.h"


int main()
{
    char* input;
    Node* root;
    char target;


    /*
        입력 문자열도 동적으로 할당
    */
    input = (char*)malloc(sizeof(char) * 100);


    if (input == NULL)
    {
        printf("메모리 할당 실패\n");
        return 1;
    }


    printf("이진트리를 입력하세요: ");
    scanf("%99s", input);


    root = buildTree(input);


    if (root == NULL)
    {
        printf("Error: 잘못된 이진트리 형식입니다.\n");

        free(input);

        return 1;
    }


    printf("\n===== 이진트리 =====\n");
    printTree(root);


    printTreeInfo(root);

    printTreeType(root);


    printf("\n특정 노드를 입력하세요: ");
    scanf(" %c", &target);


    {
        Node* parent;
        Node* targetNode;


        targetNode = findNode(root, target);


        if (targetNode == NULL)
        {
            printf("해당 노드가 존재하지 않습니다.\n");
        }
        else
        {
            parent = findParent(root, target);


            if (parent == NULL)
                printf("%c의 부모 : 없음 (루트)\n",
                    target);
            else
                printf("%c의 부모 : %c\n",
                    target,
                    parent->data);


            printChildren(root, target);

            printSibling(root, target);
        }
    }

    printf("포인터 구현 메모리 사용량 : %zu bytes\n",
        sizeof(Node) * countNodes(root));
    /*
        동적 할당한 메모리 해제
    */
    freeTree(root);
    free(input);


    return 0;
}