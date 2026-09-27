#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "TreeHw04.h"


/* =========================================================
   노드 생성
   ========================================================= */

BNode* create_node(char data)
{
    BNode* node = (BNode*)malloc(sizeof(BNode));

    if (node == NULL)
    {
        printf("메모리 할당 실패\n");
        exit(1);
    }

    node->data = data;
    node->left = NULL;
    node->right = NULL;

    return node;
}


/* =========================================================
   괄호 표기법으로 트리 생성
   예:
   A(B(D,E),C(,F))
   ========================================================= */

BNode* build_tree(const char* input)
{
    BNode* root = NULL;

    /*
       부모 노드와 현재 어느 자식을
       넣어야 하는지를 저장하는 스택
    */

    typedef struct
    {
        BNode* node;
        int childCount;
    } ParentInfo;

    ParentInfo* stack = NULL;
    int top = -1;
    int capacity = 0;

    BNode* current = NULL;

    int expectNode = 1;
    int length = strlen(input);

    for (int i = 0; i < length; i++)
    {
        char ch = input[i];

        /* 공백 무시 */
        if (isspace((unsigned char)ch))
            continue;


        /* =================================================
           노드
           ================================================= */

        if (isupper((unsigned char)ch))
        {
            if (!expectNode)
            {
                printf("Error: 잘못된 트리 형식입니다.\n");

                free(stack);
                destroy_tree(root);

                return NULL;
            }

            BNode* newNode = create_node(ch);

            /* 첫 번째 노드 = 루트 */
            if (root == NULL)
            {
                root = newNode;
            }
            else
            {
                if (top < 0)
                {
                    printf("Error: 잘못된 트리 형식입니다.\n");

                    free(newNode);
                    free(stack);
                    destroy_tree(root);

                    return NULL;
                }

                ParentInfo* parent = &stack[top];

                /*
                   childCount
                   0 = 왼쪽 자식 자리
                   1 = 오른쪽 자식 자리
                   2 = 자식 둘 다 사용
                */

                if (parent->childCount == 0)
                {
                    if (parent->node->left != NULL)
                    {
                        printf("Error: 왼쪽 자식이 이미 존재합니다.\n");

                        free(newNode);
                        free(stack);
                        destroy_tree(root);

                        return NULL;
                    }

                    parent->node->left = newNode;
                    parent->childCount++;
                }
                else if (parent->childCount == 1)
                {
                    if (parent->node->right != NULL)
                    {
                        printf("Error: 오른쪽 자식이 이미 존재합니다.\n");

                        free(newNode);
                        free(stack);
                        destroy_tree(root);

                        return NULL;
                    }

                    parent->node->right = newNode;
                    parent->childCount++;
                }
                else
                {
                    printf("Error: 자식이 2개를 초과했습니다.\n");

                    free(newNode);
                    free(stack);
                    destroy_tree(root);

                    return NULL;
                }
            }

            current = newNode;
            expectNode = 0;
        }


        /* =================================================
           (
           ================================================= */

        else if (ch == '(')
        {
            if (current == NULL || expectNode)
            {
                printf("Error: 잘못된 '(' 위치입니다.\n");

                free(stack);
                destroy_tree(root);

                return NULL;
            }

            /*
               스택 공간이 부족하면 2배로 증가
            */

            if (top + 1 >= capacity)
            {
                int newCapacity;

                if (capacity == 0)
                    newCapacity = 4;
                else
                    newCapacity = capacity * 2;

                ParentInfo* temp =
                    (ParentInfo*)realloc(
                        stack,
                        sizeof(ParentInfo) * newCapacity
                    );

                if (temp == NULL)
                {
                    printf("메모리 할당 실패\n");

                    free(stack);
                    destroy_tree(root);

                    return NULL;
                }

                stack = temp;
                capacity = newCapacity;
            }

            /*
               현재 노드를 부모로 등록
            */

            stack[++top].node = current;
            stack[top].childCount = 0;

            expectNode = 1;
            current = NULL;
        }

           /* =================================================
              ,
              ================================================= */

        else if (ch == ',')
        {
            if (top < 0)
            {
                printf("Error: 잘못된 ',' 위치입니다.\n");

                free(stack);
                destroy_tree(root);

                return NULL;
            }

            /*
               C(,F) 형태:
               왼쪽 자식이 없는 경우
               '(' 다음에 바로 ','가 나온다.

               childCount
               0 = 왼쪽 자식 없음
               1 = 왼쪽 자식이 있거나 왼쪽을 건너뜀
               2 = 왼쪽/오른쪽 모두 사용
            */

            if (stack[top].childCount == 0)
            {
                /*
                   현재까지 왼쪽 자식이 없으므로
                   왼쪽을 비워두고 오른쪽 자리를 준비
                */
                stack[top].childCount = 1;
            }
            else if (stack[top].childCount == 1)
            {
                /*
                   왼쪽 자식이 이미 존재하는 경우
                   이제 오른쪽 자식을 받을 준비
                */
            }
            else
            {
                printf("Error: 잘못된 ',' 위치입니다.\n");

                free(stack);
                destroy_tree(root);

                return NULL;
            }

            expectNode = 1;
            current = NULL;
            }
        /* =================================================
           )
           ================================================= */

        else if (ch == ')')
        {
            if (top < 0)
            {
                printf("Error: 닫는 괄호가 잘못되었습니다.\n");

                free(stack);
                destroy_tree(root);

                return NULL;
            }

            /*
               자식이 없는 경우도 허용
            */

            if (expectNode)
            {
                /*
                   예: A(B(,))
                   같은 잘못된 입력 방지
                */

                printf("Error: 잘못된 ')' 위치입니다.\n");

                free(stack);
                destroy_tree(root);

                return NULL;
            }

            top--;

            if (top >= 0)
            {
                current = stack[top].node;
            }
            else
            {
                current = root;
            }

            expectNode = 0;
        }


        /* =================================================
           잘못된 문자
           ================================================= */

        else
        {
            printf("Error: 허용되지 않는 문자가 있습니다.\n");

            free(stack);
            destroy_tree(root);

            return NULL;
        }
    }


    /* =====================================================
       최종 검사
       ===================================================== */

    if (root == NULL)
    {
        printf("Error: 트리가 비어 있습니다.\n");

        free(stack);

        return NULL;
    }

    if (top != -1)
    {
        printf("Error: 괄호가 제대로 닫히지 않았습니다.\n");

        free(stack);
        destroy_tree(root);

        return NULL;
    }

    if (expectNode)
    {
        printf("Error: 노드가 필요한 위치입니다.\n");

        free(stack);
        destroy_tree(root);

        return NULL;
    }

    free(stack);

    return root;
}


/* =========================================================
   트리 구조 출력
   재귀 사용 X
   ========================================================= */

void print_tree(BNode* root)
{
    if (root == NULL)
    {
        printf("트리가 비어 있습니다.\n");
        return;
    }

    typedef struct
    {
        BNode* node;
        char* prefix;
        int isRoot;
    } PrintInfo;

    PrintInfo* stack = NULL;

    int top = -1;
    int capacity = 0;


    /* 루트 추가 */

    capacity = 4;

    stack = (PrintInfo*)malloc(
        sizeof(PrintInfo) * capacity
    );

    if (stack == NULL)
    {
        printf("메모리 할당 실패\n");
        return;
    }

    stack[++top].node = root;
    stack[top].prefix = (char*)malloc(1);
    stack[top].prefix[0] = '\0';
    stack[top].isRoot = 1;


    while (top >= 0)
    {
        PrintInfo current = stack[top--];

        BNode* node = current.node;

        if (current.isRoot)
        {
            printf("%c\n", node->data);
        }
        else
        {
            printf("%s+---%c\n",
                current.prefix,
                node->data);
        }


        /*
           오른쪽 자식을 먼저 스택에 넣는다.

           스택은 LIFO이므로
           나중에 넣은 왼쪽 자식이 먼저 출력된다.
        */


        int hasRight = (node->right != NULL);
        int hasLeft = (node->left != NULL);


        /* 오른쪽 자식 */

        if (hasRight)
        {
            int prefixLength = strlen(current.prefix);

            char* newPrefix =
                (char*)malloc(prefixLength + 5);

            strcpy(newPrefix, current.prefix);

            if (hasLeft)
                strcat(newPrefix, "|   ");
            else
                strcat(newPrefix, "    ");


            if (top + 1 >= capacity)
            {
                capacity *= 2;

                PrintInfo* temp =
                    (PrintInfo*)realloc(
                        stack,
                        sizeof(PrintInfo) * capacity
                    );

                if (temp == NULL)
                {
                    free(newPrefix);
                    free(current.prefix);
                    free(stack);

                    printf("메모리 할당 실패\n");
                    return;
                }

                stack = temp;
            }

            stack[++top].node = node->right;
            stack[top].prefix = newPrefix;
            stack[top].isRoot = 0;
        }


        /* 왼쪽 자식 */

        if (hasLeft)
        {
            int prefixLength = strlen(current.prefix);

            char* newPrefix =
                (char*)malloc(prefixLength + 5);

            strcpy(newPrefix, current.prefix);

            if (hasRight)
                strcat(newPrefix, "|   ");
            else
                strcat(newPrefix, "    ");


            if (top + 1 >= capacity)
            {
                capacity *= 2;

                PrintInfo* temp =
                    (PrintInfo*)realloc(
                        stack,
                        sizeof(PrintInfo) * capacity
                    );

                if (temp == NULL)
                {
                    free(newPrefix);
                    free(current.prefix);
                    free(stack);

                    printf("메모리 할당 실패\n");
                    return;
                }

                stack = temp;
            }

            stack[++top].node = node->left;
            stack[top].prefix = newPrefix;
            stack[top].isRoot = 0;
        }


        free(current.prefix);
    }

    free(stack);
}


/* =========================================================
   전위 순회
   Root → Left → Right
   ========================================================= */

void preorder(BNode* root)
{
    if (root == NULL)
        return;

    BNode** stack = NULL;

    int top = -1;
    int capacity = 0;


    capacity = 4;

    stack = (BNode**)malloc(
        sizeof(BNode*) * capacity
    );

    if (stack == NULL)
    {
        printf("메모리 할당 실패\n");
        return;
    }

    stack[++top] = root;


    while (top >= 0)
    {
        BNode* current = stack[top--];

        printf("%c ", current->data);


        /*
           오른쪽을 먼저 넣어야
           왼쪽이 먼저 나온다.
        */

        if (current->right != NULL)
        {
            if (top + 1 >= capacity)
            {
                capacity *= 2;

                BNode** temp =
                    (BNode**)realloc(
                        stack,
                        sizeof(BNode*) * capacity
                    );

                if (temp == NULL)
                {
                    free(stack);
                    printf("\n메모리 할당 실패\n");
                    return;
                }

                stack = temp;
            }

            stack[++top] = current->right;
        }


        if (current->left != NULL)
        {
            if (top + 1 >= capacity)
            {
                capacity *= 2;

                BNode** temp =
                    (BNode**)realloc(
                        stack,
                        sizeof(BNode*) * capacity
                    );

                if (temp == NULL)
                {
                    free(stack);
                    printf("\n메모리 할당 실패\n");
                    return;
                }

                stack = temp;
            }

            stack[++top] = current->left;
        }
    }

    free(stack);
}


/* =========================================================
   중위 순회
   Left → Root → Right
   ========================================================= */

void inorder(BNode* root)
{
    BNode** stack = NULL;

    int top = -1;
    int capacity = 4;

    stack = (BNode**)malloc(
        sizeof(BNode*) * capacity
    );

    if (stack == NULL)
    {
        printf("메모리 할당 실패\n");
        return;
    }

    BNode* current = root;


    while (current != NULL || top >= 0)
    {
        /*
           왼쪽으로 계속 내려간다.
        */

        while (current != NULL)
        {
            if (top + 1 >= capacity)
            {
                capacity *= 2;

                BNode** temp =
                    (BNode**)realloc(
                        stack,
                        sizeof(BNode*) * capacity
                    );

                if (temp == NULL)
                {
                    free(stack);
                    printf("\n메모리 할당 실패\n");
                    return;
                }

                stack = temp;
            }

            stack[++top] = current;

            current = current->left;
        }


        /*
           가장 마지막에 저장한 노드를 방문
        */

        current = stack[top--];

        printf("%c ", current->data);


        /*
           오른쪽으로 이동
        */

        current = current->right;
    }

    free(stack);
}


/* =========================================================
   후위 순회
   Left → Right → Root

   스택 2개를 사용하는 방법
   ========================================================= */

void postorder(BNode* root)
{
    if (root == NULL)
        return;

    BNode** stack1 = NULL;
    BNode** stack2 = NULL;

    int top1 = -1;
    int top2 = -1;

    int capacity1 = 4;
    int capacity2 = 4;


    stack1 = (BNode**)malloc(
        sizeof(BNode*) * capacity1
    );

    stack2 = (BNode**)malloc(
        sizeof(BNode*) * capacity2
    );

    if (stack1 == NULL || stack2 == NULL)
    {
        free(stack1);
        free(stack2);

        printf("메모리 할당 실패\n");
        return;
    }


    stack1[++top1] = root;


    while (top1 >= 0)
    {
        BNode* current = stack1[top1--];


        /* stack2에 저장 */

        if (top2 + 1 >= capacity2)
        {
            capacity2 *= 2;

            BNode** temp =
                (BNode**)realloc(
                    stack2,
                    sizeof(BNode*) * capacity2
                );

            if (temp == NULL)
            {
                free(stack1);
                free(stack2);

                printf("메모리 할당 실패\n");
                return;
            }

            stack2 = temp;
        }

        stack2[++top2] = current;


        /*
           왼쪽과 오른쪽을 stack1에 저장
        */

        if (current->left != NULL)
        {
            if (top1 + 1 >= capacity1)
            {
                capacity1 *= 2;

                BNode** temp =
                    (BNode**)realloc(
                        stack1,
                        sizeof(BNode*) * capacity1
                    );

                if (temp == NULL)
                {
                    free(stack1);
                    free(stack2);

                    printf("메모리 할당 실패\n");
                    return;
                }

                stack1 = temp;
            }

            stack1[++top1] = current->left;
        }


        if (current->right != NULL)
        {
            if (top1 + 1 >= capacity1)
            {
                capacity1 *= 2;

                BNode** temp =
                    (BNode**)realloc(
                        stack1,
                        sizeof(BNode*) * capacity1
                    );

                if (temp == NULL)
                {
                    free(stack1);
                    free(stack2);

                    printf("메모리 할당 실패\n");
                    return;
                }

                stack1 = temp;
            }

            stack1[++top1] = current->right;
        }
    }


    /*
       stack2를 역순으로 출력하면
       Left → Right → Root가 된다.
    */

    while (top2 >= 0)
    {
        printf("%c ", stack2[top2--]->data);
    }


    free(stack1);
    free(stack2);
}


/* =========================================================
   트리 메모리 해제
   재귀 없이 반복문 사용
   ========================================================= */

void destroy_tree(BNode* root)
{
    if (root == NULL)
        return;

    BNode** stack = NULL;

    int top = -1;
    int capacity = 4;


    stack = (BNode**)malloc(
        sizeof(BNode*) * capacity
    );

    if (stack == NULL)
        return;


    stack[++top] = root;


    while (top >= 0)
    {
        BNode* current = stack[top--];


        if (current->left != NULL)
        {
            if (top + 1 >= capacity)
            {
                capacity *= 2;

                BNode** temp =
                    (BNode**)realloc(
                        stack,
                        sizeof(BNode*) * capacity
                    );

                if (temp == NULL)
                {
                    free(stack);
                    return;
                }

                stack = temp;
            }

            stack[++top] = current->left;
        }


        if (current->right != NULL)
        {
            if (top + 1 >= capacity)
            {
                capacity *= 2;

                BNode** temp =
                    (BNode**)realloc(
                        stack,
                        sizeof(BNode*) * capacity
                    );

                if (temp == NULL)
                {
                    free(stack);
                    return;
                }

                stack = temp;
            }

            stack[++top] = current->right;
        }


        free(current);
    }


    free(stack);
}