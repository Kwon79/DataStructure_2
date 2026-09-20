#include <stdio.h>
#include <stdlib.h>
#include "BinaryTreePointer.h"


/* =========================================
   노드 생성
   ========================================= */

Node* createNode(char data)
{
    Node* newNode;

    newNode = (Node*)malloc(sizeof(Node));

    if (newNode == NULL)
        return NULL;

    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;
}


/* =========================================
   동적 스택
   ========================================= */

typedef struct StackNode
{
    Node* node;
    int state;

    struct StackNode* next;
} StackNode;


/* 스택에 삽입 */
void push(StackNode** top, Node* node, int state)
{
    StackNode* newStackNode;

    newStackNode = (StackNode*)malloc(sizeof(StackNode));

    if (newStackNode == NULL)
        return;

    newStackNode->node = node;
    newStackNode->state = state;
    newStackNode->next = *top;

    *top = newStackNode;
}


/* 스택에서 제거 */
StackNode* pop(StackNode** top)
{
    StackNode* temp;

    if (*top == NULL)
        return NULL;

    temp = *top;
    *top = (*top)->next;

    return temp;
}


/* 스택 전체 해제 */
void freeStack(StackNode* top)
{
    StackNode* temp;

    while (top != NULL)
    {
        temp = top;
        top = top->next;
        free(temp);
    }
}


/* =========================================
   트리 생성
   ========================================= */

Node* buildTree(const char* input)
{
    Node* root = NULL;
    Node* currentNode = NULL;

    StackNode* stack = NULL;

    int i = 0;

    /*
        state

        0 : 왼쪽 자식 입력 전
        1 : 왼쪽 자식 입력 완료
        2 : 오른쪽 자식 입력 대기
        3 : 오른쪽 자식 입력 완료
    */

    int used[26] = { 0 };


    while (input[i] != '\0')
    {
        char ch = input[i];


        /* 공백 무시 */
        if (ch == ' ')
        {
            i++;
            continue;
        }


        /* =====================================
           노드
           ===================================== */

        if (ch >= 'A' && ch <= 'Z')
        {
            Node* newNode;

            /* 중복 노드 검사 */
            if (used[ch - 'A'])
            {
                freeStack(stack);
                freeTree(root);
                return NULL;
            }

            used[ch - 'A'] = 1;


            /* 노드 동적 할당 */
            newNode = createNode(ch);

            if (newNode == NULL)
            {
                freeStack(stack);
                freeTree(root);
                return NULL;
            }


            /* 첫 번째 노드는 루트 */
            if (root == NULL)
            {
                root = newNode;
                currentNode = newNode;
            }
            else
            {
                /*
                    부모 스택이 없으면
                    새로운 루트를 만들려고 하는 것
                */
                if (stack == NULL)
                {
                    free(newNode);
                    freeTree(root);
                    return NULL;
                }


                /*
                    스택의 가장 위 = 현재 부모
                */
                if (stack->state == 0)
                {
                    stack->node->left = newNode;
                    stack->state = 1;
                }
                else if (stack->state == 2)
                {
                    stack->node->right = newNode;
                    stack->state = 3;
                }
                else
                {
                    free(newNode);
                    freeStack(stack);
                    freeTree(root);
                    return NULL;
                }

                currentNode = newNode;
            }

            i++;
            continue;
        }


        /* =====================================
           '('
           ===================================== */

        if (ch == '(')
        {
            /*
                현재 노드가 있어야 함
            */
            if (currentNode == NULL)
            {
                freeStack(stack);
                freeTree(root);
                return NULL;
            }


            /*
                현재 노드를 부모 스택에 저장
            */
            push(&stack, currentNode, 0);

            i++;
            continue;
        }


        /* =====================================
           ','
           ===================================== */

        if (ch == ',')
        {
            if (stack == NULL)
            {
                freeStack(stack);
                freeTree(root);
                return NULL;
            }


            /*
                A(,B)

                왼쪽 자식이 비어 있음
            */
            if (stack->state == 0)
            {
                stack->state = 2;
            }


            /*
                A(B,C)

                왼쪽 자식이 존재
            */
            else if (stack->state == 1)
            {
                stack->state = 2;
            }

            else
            {
                freeStack(stack);
                freeTree(root);
                return NULL;
            }

            i++;
            continue;
        }


        /* =====================================
           ')'
           ===================================== */

        if (ch == ')')
        {
            StackNode* temp;


            if (stack == NULL)
            {
                freeStack(stack);
                freeTree(root);
                return NULL;
            }


            /*
                A()

                자식이 하나도 없음
            */
            if (stack->state == 0)
            {
                freeStack(stack);
                freeTree(root);
                return NULL;
            }


            /*
                A(,)

                오른쪽 자식이 없음
            */
            if (stack->state == 2)
            {
                freeStack(stack);
                freeTree(root);
                return NULL;
            }


            /*
                부모 스택에서 제거
            */
            temp = pop(&stack);

            /*
                닫은 부모로 돌아감
            */
            currentNode = temp->node;

            free(temp);

            i++;
            continue;
        }


        /* 알 수 없는 문자 */
        freeStack(stack);
        freeTree(root);
        return NULL;
    }


    /*
        괄호가 닫히지 않은 경우
    */
    if (stack != NULL)
    {
        freeStack(stack);
        freeTree(root);
        return NULL;
    }


    return root;
}


/* =========================================
   트리 출력
   ========================================= */

void printTreeRecursive(Node* root, int depth)
{
    int i;

    if (root == NULL)
        return;


    for (i = 0; i < depth; i++)
        printf("    ");


    if (depth > 0)
        printf("+---");


    printf("%c\n", root->data);


    printTreeRecursive(root->left, depth + 1);
    printTreeRecursive(root->right, depth + 1);
}


void printTree(Node* root)
{
    printTreeRecursive(root, 0);
}


/* =========================================
   전체 노드 수
   ========================================= */

int countNodes(Node* root)
{
    if (root == NULL)
        return 0;

    return 1
        + countNodes(root->left)
        + countNodes(root->right);
}


/* =========================================
   단말 노드 수
   ========================================= */

int countLeafNodes(Node* root)
{
    if (root == NULL)
        return 0;


    if (root->left == NULL &&
        root->right == NULL)
    {
        return 1;
    }


    return countLeafNodes(root->left)
        + countLeafNodes(root->right);
}


/* =========================================
   비단말 노드 수
   ========================================= */

int countNonLeafNodes(Node* root)
{
    if (root == NULL)
        return 0;


    if (root->left == NULL &&
        root->right == NULL)
    {
        return 0;
    }


    return 1
        + countNonLeafNodes(root->left)
        + countNonLeafNodes(root->right);
}


/* =========================================
   높이
   ========================================= */

int getHeight(Node* root)
{
    int leftHeight;
    int rightHeight;


    if (root == NULL)
        return -1;


    leftHeight = getHeight(root->left);
    rightHeight = getHeight(root->right);


    if (leftHeight > rightHeight)
        return leftHeight + 1;


    return rightHeight + 1;
}


/* =========================================
   차수
   ========================================= */

int getDegree(Node* root)
{
    int degree = 0;
    int leftDegree;
    int rightDegree;


    if (root == NULL)
        return 0;


    if (root->left != NULL)
        degree++;


    if (root->right != NULL)
        degree++;


    leftDegree = getDegree(root->left);
    rightDegree = getDegree(root->right);


    if (leftDegree > degree)
        degree = leftDegree;


    if (rightDegree > degree)
        degree = rightDegree;


    return degree;
}


/* =========================================
   노드 찾기
   ========================================= */

Node* findNode(Node* root, char data)
{
    Node* result;


    if (root == NULL)
        return NULL;


    if (root->data == data)
        return root;


    result = findNode(root->left, data);

    if (result != NULL)
        return result;


    return findNode(root->right, data);
}


/* =========================================
   부모 찾기
   ========================================= */

Node* findParent(Node* root, char data)
{
    Node* result;


    if (root == NULL)
        return NULL;


    if ((root->left != NULL &&
        root->left->data == data) ||

        (root->right != NULL &&
            root->right->data == data))
    {
        return root;
    }


    result = findParent(root->left, data);

    if (result != NULL)
        return result;


    return findParent(root->right, data);
}


/* =========================================
   자식 출력
   ========================================= */

void printChildren(Node* root, char data)
{
    Node* node = findNode(root, data);


    if (node == NULL)
    {
        printf("%c 노드를 찾을 수 없습니다.\n", data);
        return;
    }


    printf("%c의 자식 : ", data);


    if (node->left == NULL &&
        node->right == NULL)
    {
        printf("없음\n");
        return;
    }


    if (node->left != NULL)
        printf("%c ", node->left->data);


    if (node->right != NULL)
        printf("%c", node->right->data);


    printf("\n");
}


/* =========================================
   형제 출력
   ========================================= */

void printSibling(Node* root, char data)
{
    Node* parent;


    if (root == NULL ||
        root->data == data)
    {
        printf("%c의 형제 : 없음\n", data);
        return;
    }


    parent = findParent(root, data);


    if (parent == NULL)
    {
        printf("%c의 형제 : 없음\n", data);
        return;
    }


    printf("%c의 형제 : ", data);


    if (parent->left != NULL &&
        parent->left->data == data)
    {
        if (parent->right != NULL)
            printf("%c\n", parent->right->data);
        else
            printf("없음\n");
    }
    else
    {
        if (parent->left != NULL)
            printf("%c\n", parent->left->data);
        else
            printf("없음\n");
    }
}


/* =========================================
   완전 이진트리
   ========================================= */

int checkComplete(Node* root, int index, int nodeCount)
{
    if (root == NULL)
        return 1;


    if (index >= nodeCount)
        return 0;


    return checkComplete(root->left,
        2 * index + 1,
        nodeCount)
        &&
        checkComplete(root->right,
            2 * index + 2,
            nodeCount);
}


int isComplete(Node* root)
{
    int nodeCount;


    if (root == NULL)
        return 1;


    nodeCount = countNodes(root);


    return checkComplete(root, 0, nodeCount);
}


/* =========================================
   포화 이진트리
   ========================================= */

int isFull(Node* root)
{
    int height;
    int nodeCount;
    int expected;


    if (root == NULL)
        return 1;


    height = getHeight(root);
    nodeCount = countNodes(root);


    expected = (1 << (height + 1)) - 1;


    return nodeCount == expected;
}


/* =========================================
   편향 이진트리
   ========================================= */

int isSkewed(Node* root)
{
    if (root == NULL)
        return 1;


    /*
        양쪽 자식이 모두 있으면
        편향 이진트리가 아님
    */
    if (root->left != NULL &&
        root->right != NULL)
    {
        return 0;
    }


    return isSkewed(root->left)
        && isSkewed(root->right);
}


/* =========================================
   트리 정보 출력
   ========================================= */

void printTreeInfo(Node* root)
{
    printf("\n===== 이진트리 정보 =====\n");

    printf("전체 노드 수 : %d\n",
        countNodes(root));

    printf("단말 노드 수 : %d\n",
        countLeafNodes(root));

    printf("비단말 노드 수 : %d\n",
        countNonLeafNodes(root));

    printf("트리의 높이 : %d\n",
        getHeight(root));

    printf("트리의 차수 : %d\n",
        getDegree(root));
}


/* =========================================
   트리 종류
   ========================================= */

void printTreeType(Node* root)
{
    printf("\n===== 이진트리 종류 =====\n");


    if (isComplete(root))
        printf("완전 이진트리 : YES\n");
    else
        printf("완전 이진트리 : NO\n");


    if (isFull(root))
        printf("포화 이진트리 : YES\n");
    else
        printf("포화 이진트리 : NO\n");


    if (isSkewed(root))
        printf("편향 이진트리 : YES\n");
    else
        printf("편향 이진트리 : NO\n");
}


/* =========================================
   메모리 해제
   ========================================= */

void freeTree(Node* root)
{
    if (root == NULL)
        return;


    freeTree(root->left);
    freeTree(root->right);


    free(root);
}