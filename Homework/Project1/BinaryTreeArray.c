#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "BinaryTreeArray.h"


/* ========================================
   트리 초기화
   ======================================== */

void initTree(ArrayTree* t)
{
    for (int i = 0; i < MAX; i++)
    {
        t->tree[i] = '\0';
    }

    t->size = 0;
}


/* ========================================
   트리 생성
   ======================================== */

int buildTree(ArrayTree* t, const char* input)
{
    int indexStack[MAX];
    int stateStack[MAX];

    int top = -1;
    int currentIndex = 0;
    int nodeCount = 0;

    int used[26] = { 0 };

    /*
        state 의미
        0 : 왼쪽 자식 입력 전
        1 : 왼쪽 자식 입력 완료
        2 : 콤마 입력 후, 오른쪽 자식 입력 전
        3 : 오른쪽 자식 입력 완료
    */

    int i = 0;

    while (input[i] != '\0')
    {
        char ch = input[i];

        /* 공백 무시 */
        if (ch == ' ')
        {
            i++;
            continue;
        }

        /* 알파벳 노드 */
        if (ch >= 'A' && ch <= 'Z')
        {
            int newIndex;

            /* 이미 같은 노드가 존재하면 오류 */
            if (used[ch - 'A'])
                return 0;

            used[ch - 'A'] = 1;

            /*
                첫 번째 노드는 반드시 루트
            */
            if (nodeCount == 0)
            {
                if (ch != input[0])
                    return 0;

                newIndex = 0;
            }
            else
            {
                /* 부모가 없으면 오류 */
                if (top < 0)
                    return 0;

                int parentIndex = indexStack[top];

                /* 왼쪽 자식 */
                if (stateStack[top] == 0)
                {
                    newIndex = 2 * parentIndex + 1;
                    stateStack[top] = 1;
                }
                /* 오른쪽 자식 */
                else if (stateStack[top] == 2)
                {
                    newIndex = 2 * parentIndex + 2;
                    stateStack[top] = 3;
                }
                else
                {
                    return 0;
                }
            }

            /* 배열 범위 검사 */
            if (newIndex >= MAX)
                return 0;

            t->tree[newIndex] = ch;

            if (newIndex + 1 > t->size)
                t->size = newIndex + 1;

            currentIndex = newIndex;
            nodeCount++;

            i++;
            continue;
        }

        /* '(' */
        if (ch == '(')
        {
            /*
                현재 노드가 있어야 '(' 가능
            */
            if (nodeCount == 0)
                return 0;

            /*
                현재 노드를 부모 스택에 저장
            */
            top++;

            if (top >= MAX)
                return 0;

            indexStack[top] = currentIndex;
            stateStack[top] = 0;

            i++;
            continue;
        }

        /* ',' */
        if (ch == ',')
        {
            if (top < 0)
                return 0;

            /*
                왼쪽 자식이 비어 있는 경우
                A(,B)
            */
            if (stateStack[top] == 0)
            {
                stateStack[top] = 2;
            }
            /*
                왼쪽 자식이 있고 오른쪽 자식을 기다리는 경우
                A(B,C)
            */
            else if (stateStack[top] == 1)
            {
                stateStack[top] = 2;
            }
            else
            {
                return 0;
            }

            i++;
            continue;
        }

        /* ')' */
        if (ch == ')')
        {
            if (top < 0)
                return 0;

            /*
                현재 부모의 상태가
                0이면 자식이 하나도 없음
                2이면 콤마 뒤 오른쪽 자식도 없음

                A()
                A(,)
                같은 잘못된 형태를 막음
            */

            if (stateStack[top] == 0)
                return 0;

            if (stateStack[top] == 2)
                return 0;

            /*
                1 : 왼쪽 자식만 존재
                3 : 왼쪽 + 오른쪽 존재
            */

            top--;

            /*
                부모 노드로 돌아감
            */
            if (top >= 0)
                currentIndex = indexStack[top];

            i++;
            continue;
        }

        /* 그 외 문자는 오류 */
        return 0;
    }

    /*
        괄호가 전부 닫혔는지 검사
    */
    if (top != -1)
        return 0;

    if (nodeCount == 0)
        return 0;

    return 1;
}


/* ========================================
   트리 출력
   ======================================== */

static void printTreeRecursive(
    const ArrayTree* t,
    int index,
    int depth)
{
    if (index >= MAX || t->tree[index] == '\0')
    {
        return;
    }


    // 루트
    if (depth == 0)
    {
        printf("%c\n", t->tree[index]);
    }

    // 자식
    else
    {
        for (int i = 0; i < depth - 1; i++)
        {
            printf("    ");
        }

        printf("+---%c\n", t->tree[index]);
    }


    // 왼쪽 자식
    printTreeRecursive(
        t,
        2 * index + 1,
        depth + 1
    );

    // 오른쪽 자식
    printTreeRecursive(
        t,
        2 * index + 2,
        depth + 1
    );
}


void printTree(const ArrayTree* t)
{
    if (t->size == 0)
    {
        printf("트리가 비어 있습니다.\n");
        return;
    }

    printTreeRecursive(t, 0, 0);
}


/* ========================================
   전체 노드 수
   ======================================== */

static int countNodes(const ArrayTree* t)
{
    int count = 0;

    for (int i = 0; i < MAX; i++)
    {
        if (t->tree[i] != '\0')
        {
            count++;
        }
    }

    return count;
}


/* ========================================
   단말 노드 수
   ======================================== */

static int countLeafNodes(const ArrayTree* t)
{
    int count = 0;

    for (int i = 0; i < MAX; i++)
    {
        if (t->tree[i] == '\0')
        {
            continue;
        }


        int left = 2 * i + 1;
        int right = 2 * i + 2;

        int hasLeft = 0;
        int hasRight = 0;


        if (left < MAX && t->tree[left] != '\0')
        {
            hasLeft = 1;
        }


        if (right < MAX && t->tree[right] != '\0')
        {
            hasRight = 1;
        }


        if (!hasLeft && !hasRight)
        {
            count++;
        }
    }

    return count;
}


/* ========================================
   비단말 노드 수
   ======================================== */

static int countNonLeafNodes(const ArrayTree* t)
{
    return countNodes(t) - countLeafNodes(t);
}


/* ========================================
   트리 높이
   ======================================== */

static int getHeightRecursive(
    const ArrayTree* t,
    int index)
{
    if (index >= MAX || t->tree[index] == '\0')
    {
        return -1;
    }


    int leftHeight =
        getHeightRecursive(
            t,
            2 * index + 1
        );

    int rightHeight =
        getHeightRecursive(
            t,
            2 * index + 2
        );


    if (leftHeight > rightHeight)
    {
        return leftHeight + 1;
    }

    return rightHeight + 1;
}


static int getHeight(const ArrayTree* t)
{
    if (t->size == 0)
    {
        return -1;
    }

    return getHeightRecursive(t, 0);
}


/* ========================================
   트리 차수
   ======================================== */

static int getDegree(const ArrayTree* t)
{
    int degree = 0;

    for (int i = 0; i < MAX; i++)
    {
        if (t->tree[i] == '\0')
        {
            continue;
        }


        int childCount = 0;

        int left = 2 * i + 1;
        int right = 2 * i + 2;


        if (left < MAX && t->tree[left] != '\0')
        {
            childCount++;
        }


        if (right < MAX && t->tree[right] != '\0')
        {
            childCount++;
        }


        if (childCount > degree)
        {
            degree = childCount;
        }
    }

    return degree;
}


/* ========================================
   완전 이진트리
   ======================================== */

static int isComplete(const ArrayTree* t)
{
    int foundEmpty = 0;

    for (int i = 0; i < MAX; i++)
    {
        if (t->tree[i] == '\0')
        {
            foundEmpty = 1;
        }
        else
        {
            // 앞에 빈 공간이 있었는데
            // 뒤에서 다시 노드가 나오면 불완전
            if (foundEmpty)
            {
                return 0;
            }
        }
    }

    return 1;
}


/* ========================================
   포화 이진트리
   ======================================== */

static int isFull(const ArrayTree* t)
{
    int height = getHeight(t);

    if (height < 0)
    {
        return 0;
    }


    int expectedNodes =
        (1 << (height + 1)) - 1;


    return countNodes(t) == expectedNodes;
}


/* ========================================
   편향 이진트리
   ======================================== */

static int isSkewed(const ArrayTree* t)
{
    for (int i = 0; i < MAX; i++)
    {
        if (t->tree[i] == '\0')
        {
            continue;
        }


        int left = 2 * i + 1;
        int right = 2 * i + 2;

        int hasLeft = 0;
        int hasRight = 0;


        if (left < MAX && t->tree[left] != '\0')
        {
            hasLeft = 1;
        }


        if (right < MAX && t->tree[right] != '\0')
        {
            hasRight = 1;
        }


        // 두 자식이 모두 존재하면 편향 아님
        if (hasLeft && hasRight)
        {
            return 0;
        }
    }

    return 1;
}


/* ========================================
   트리 정보 출력
   ======================================== */

void printTreeInfo(const ArrayTree* t)
{
    printf("\n===== 트리 정보 =====\n");

    printf(
        "전체 노드 수 : %d\n",
        countNodes(t)
    );

    printf(
        "단말 노드 수 : %d\n",
        countLeafNodes(t)
    );

    printf(
        "비단말 노드 수 : %d\n",
        countNonLeafNodes(t)
    );

    printf(
        "트리의 높이 : %d\n",
        getHeight(t)
    );

    printf(
        "트리의 차수 : %d\n",
        getDegree(t)
    );
}


/* ========================================
   이진트리 종류 출력
   ======================================== */

void printTreeType(const ArrayTree* t)
{
    printf("\n===== 이진트리 형태 =====\n");


    if (isComplete(t))
    {
        printf("완전 이진트리 : O\n");
    }
    else
    {
        printf("완전 이진트리 : X\n");
    }


    if (isFull(t))
    {
        printf("포화 이진트리 : O\n");
    }
    else
    {
        printf("포화 이진트리 : X\n");
    }


    if (isSkewed(t))
    {
        printf("편향 이진트리 : O\n");
    }
    else
    {
        printf("편향 이진트리 : X\n");
    }
}


/* ========================================
   특정 노드 찾기
   ======================================== */

int findNode(const ArrayTree* t, char data)
{
    for (int i = 0; i < MAX; i++)
    {
        if (t->tree[i] == data)
        {
            return i;
        }
    }

    return -1;
}


/* ========================================
   부모 출력
   ======================================== */

void printParent(const ArrayTree* t, char data)
{
    int index = findNode(t, data);


    if (index == -1)
    {
        printf("%c 노드를 찾을 수 없습니다.\n", data);
        return;
    }


    if (index == 0)
    {
        printf("%c의 부모 : 없음 (루트)\n", data);
        return;
    }


    int parent = (index - 1) / 2;


    if (t->tree[parent] != '\0')
    {
        printf(
            "%c의 부모 : %c\n",
            data,
            t->tree[parent]
        );
    }
    else
    {
        printf(
            "%c의 부모 : 없음\n",
            data
        );
    }
}


/* ========================================
   자식 출력
   ======================================== */

void printChildren(const ArrayTree* t, char data)
{
    int index = findNode(t, data);


    if (index == -1)
    {
        printf("%c 노드를 찾을 수 없습니다.\n", data);
        return;
    }


    int left = 2 * index + 1;
    int right = 2 * index + 2;


    printf("%c의 자식 : ", data);


    if ((left >= MAX || t->tree[left] == '\0') &&
        (right >= MAX || t->tree[right] == '\0'))
    {
        printf("없음\n");
        return;
    }


    if (left < MAX && t->tree[left] != '\0')
    {
        printf("%c", t->tree[left]);
    }


    if (right < MAX && t->tree[right] != '\0')
    {
        printf(" %c", t->tree[right]);
    }


    printf("\n");
}


/* ========================================
   형제 출력
   ======================================== */

void printSibling(const ArrayTree* t, char data)
{
    int index = findNode(t, data);


    if (index == -1)
    {
        printf("%c 노드를 찾을 수 없습니다.\n", data);
        return;
    }


    if (index == 0)
    {
        printf("%c의 형제 : 없음\n", data);
        return;
    }


    int parent = (index - 1) / 2;

    int sibling;


    // 왼쪽 자식이면 오른쪽이 형제
    if (index == 2 * parent + 1)
    {
        sibling = 2 * parent + 2;
    }

    // 오른쪽 자식이면 왼쪽이 형제
    else
    {
        sibling = 2 * parent + 1;
    }


    if (sibling < MAX && t->tree[sibling] != '\0')
    {
        printf(
            "%c의 형제 : %c\n",
            data,
            t->tree[sibling]
        );
    }
    else
    {
        printf(
            "%c의 형제 : 없음\n",
            data
        );
    }
}