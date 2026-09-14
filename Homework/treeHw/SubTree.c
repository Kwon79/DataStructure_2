#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX 100

int main()
{
    char input[MAX];

    // 스택
    char nodeStack[MAX];
    int countStack[MAX];

    int nodeTop = -1;
    int countTop = -1;

    // 노드 정보
    int total = 0;
    int depth = 0;
    int height = 0;
    int treeDegree = 0;

    int hasChild[26] = { 0 };

    // C의 부모와 자식
    char parentOfC = '\0';
    char childrenOfC[26];
    int cChildCount = 0;

    // 입력
    printf("트리를 입력하세요: ");
    if (!fgets(input, MAX, stdin)) return 1;
    input[strcspn(input, "\n")] = '\0';   // [수정] 개행 제거

    int len = strlen(input);

    // 다음에 노드가 와야 하는지 여부
    int expectNode = 1;

    // 가장 최근에 읽은 노드
    char lastNode = '\0';

    for (int i = 0; i < len; i++)
    {
        char ch = input[i];

        /* --------------------
           노드
           -------------------- */
        if (isupper(ch))
        {
            // 노드가 와야 하는 상황이 아니면 오류
            if (!expectNode)
            {
                printf("Error: 잘못된 트리 형식입니다.\n");
                return 1;
            }

            // [수정] "등장 순서 = 알파벳 순서" 검사는 삭제.
            // 예: A(B(E,F),C,D(G)) 처럼 실제 등장 순서(A,B,E,F,C,D,G)가
            // 알파벳 순서와 다른 정상적인 트리도 있으므로, 대문자인지만 확인한다.

            if (total >= 26)
            {
                printf("Error: 노드는 최대 26개입니다.\n");
                return 1;
            }

            // 부모가 존재하면
            if (nodeTop >= 0)
            {
                // 현재 노드의 부모
                char parent = nodeStack[nodeTop];

                hasChild[parent - 'A'] = 1;

                // 부모의 자식 수 증가
                countStack[countTop]++;

                // C의 직접 자식인지 확인
                if (parent == 'C')
                {
                    childrenOfC[cChildCount++] = ch;
                }
            }

            // C의 부모 저장
            if (ch == 'C' && nodeTop >= 0)
            {
                parentOfC = nodeStack[nodeTop];
            }

            total++;

            // [수정] 현재 노드 자신의 깊이(= 지금까지 열린 괄호 수 + 1)로 높이 계산
            // 기존 코드는 '(' 를 만나기 전에 depth를 체크해서 1씩 작게 나오는 버그가 있었음
            if (depth + 1 > height)
                height = depth + 1;

            lastNode = ch;
            expectNode = 0;
        }

        /* --------------------
           '('
           -------------------- */
        else if (ch == '(')
        {
            // 반드시 노드 다음에 와야 함
            if (expectNode || lastNode == '\0')
            {
                printf("Error: 잘못된 '(' 위치입니다.\n");
                return 1;
            }

            // 현재 노드를 부모 스택에 저장
            nodeStack[++nodeTop] = lastNode;

            // 이 부모의 자식 수를 0으로 시작
            countStack[++countTop] = 0;

            depth++;

            expectNode = 1;
        }

        /* --------------------
           ','
           -------------------- */
        else if (ch == ',')
        {
            // 괄호 안에서만 가능
            if (countTop < 0 || expectNode)
            {
                printf("Error: 잘못된 ',' 위치입니다.\n");
                return 1;
            }

            // 다음에는 새로운 노드가 와야 함
            expectNode = 1;
        }

        /* --------------------
           ')'
           -------------------- */
        else if (ch == ')')
        {
            // 괄호가 열려 있어야 함
            if (countTop < 0 || expectNode)
            {
                printf("Error: 잘못된 ')' 위치입니다.\n");
                return 1;
            }

            // 현재 부모의 자식 수로 차수 계산
            if (countStack[countTop] > treeDegree)
                treeDegree = countStack[countTop];

            // 스택에서 제거
            countTop--;
            nodeTop--;

            depth--;

            expectNode = 0;
        }

        /* --------------------
           공백 (무시)
           -------------------- */
        else if (isspace((unsigned char)ch))
        {
            continue;
        }

        /* --------------------
           그 외 문자
           -------------------- */
        else
        {
            printf("Error: 허용되지 않는 문자가 있습니다.\n");
            return 1;
        }
    }

    /* --------------------
       최종 형식 검사
       -------------------- */

       // 마지막에 노드를 기다리는 상태면 오류
    if (expectNode)
    {
        printf("Error: 잘못된 트리 형식입니다.\n");
        return 1;
    }

    // 괄호가 모두 닫혔는지 확인
    if (nodeTop != -1 || countTop != -1 || depth != 0)
    {
        printf("Error: 괄호가 올바르게 닫히지 않았습니다.\n");
        return 1;
    }

    // 노드가 하나도 없으면 오류
    if (total == 0)
    {
        printf("Error: 트리가 비어 있습니다.\n");
        return 1;
    }

    /* --------------------
       리프 / 비리프 계산
       -------------------- */

    int leaf = 0;
    int nonLeaf = 0;

    for (int i = 0; i < total; i++)
    {
        if (hasChild[i])
            nonLeaf++;
        else
            leaf++;
    }

    /* --------------------
       결과 출력
       -------------------- */

    printf("\n===== 트리 정보 =====\n");

    printf("전체 노드 수 : %d\n", total);
    printf("단말 노드 수 : %d\n", leaf);
    printf("비단말 노드 수 : %d\n", nonLeaf);
    printf("트리의 높이 : %d\n", height);
    printf("트리의 차수 : %d\n", treeDegree);

    // C의 부모
    if (parentOfC != '\0')
        printf("C의 부모 : %c\n", parentOfC);
    else
        printf("C의 부모 : 없음\n");

    // C의 자식
    printf("C의 자식 : ");

    if (cChildCount == 0)
    {
        printf("없음\n");
    }
    else
    {
        for (int i = 0; i < cChildCount; i++)
            printf("%c ", childrenOfC[i]);

        printf("\n");
    }

    /* --------------------
       계층적 트리 출력
       -------------------- */

    printf("\n===== 트리 구조 =====\n");

    depth = 0;

    for (int i = 0; i < len; i++)
    {
        char ch = input[i];

        if (isupper(ch))
        {
            // 현재 깊이만큼 들여쓰기
            if (depth == 0)
            {
                printf("%c\n", ch);
            }
            else
            {
                for (int j = 0; j < depth - 1; j++)
                    printf("    ");

                printf("+---%c\n", ch);
            }
        }
        else if (ch == '(')
        {
            depth++;
        }
        else if (ch == ')')
        {
            depth--;
        }
    }

    return 0;
}