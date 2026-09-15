#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

#define MAX_NODES 1024
#define STACK_SIZE 1024

char tree[MAX_NODES];   // 0 이면 빈 노드
char input[10000];
int pos;

// 재귀 호출 대신 사용할 명시적 스택 프레임
// phase: 이 프레임이 재진입했을 때 어느 단계부터 이어갈지 표시
//   0 = 값을 읽고 다음 처리 결정
//   1 = 콤마모드(오른쪽 자식만) 처리 후 복귀
//   2 = 왼쪽 자식 처리 후 복귀 (두번째 그룹 확인 필요)
//   3 = 오른쪽 자식 처리 후 복귀
typedef struct {
    int idx;
    int phase;
} Frame;

Frame stack[STACK_SIZE];
int top;

void push(int idx, int phase) {
    top++;
    stack[top].idx = idx;
    stack[top].phase = phase;
}

/*
 * 괄호식 문법
 *  leaf            : 값 하나만 (자식 없음)             예) A
 *  왼쪽만 존재      : 값(왼쪽서브트리)                   예) A(B)
 *  오른쪽만 존재    : 값(,오른쪽서브트리)                예) A(,C)
 *  둘 다 존재       : 값(왼쪽서브트리)(오른쪽서브트리)   예) A(B)(C)
 *
 * 재귀호출 대신, 각 노드의 "다음에 할 일"을 phase 값으로 스택에 저장해두고
 * while문으로 반복 처리(반복적 방법)한다.
 */
void buildTree(int rootIdx) {
    top = -1;
    push(rootIdx, 0);

    while (top >= 0) {
        Frame* f = &stack[top];

        if (f->idx >= MAX_NODES) {   // 배열 범위 안전장치
            top--;
            continue;
        }

        switch (f->phase) {
        case 0: {                          // 값을 읽고 다음 단계 결정
            char value = input[pos++];
            tree[f->idx] = value;

            if (input[pos] == '(') {
                pos++;                      // '(' 소비
                if (input[pos] == ',') {    // 왼쪽 자식 없음
                    pos++;                  // ',' 소비
                    f->phase = 1;
                    push(2 * f->idx + 1, 0); // 오른쪽 자식 처리 예약
                }
                else {                    // 왼쪽 자식 존재
                    f->phase = 2;
                    push(2 * f->idx, 0);    // 왼쪽 자식 처리 예약
                }
            }
            else {
                top--;                      // 자식 없음: 이 노드 완료
            }
            break;
        }
        case 1:                            // 오른쪽 전용 자식 처리 완료 후
            pos++;                          // ')' 소비
            top--;                          // 이 노드 완료
            break;

        case 2:                            // 왼쪽 자식 처리 완료 후
            pos++;                          // ')' 소비
            if (input[pos] == '(') {        // 두번째 그룹(오른쪽) 존재
                pos++;                      // '(' 소비
                f->phase = 3;
                push(2 * f->idx + 1, 0);    // 오른쪽 자식 처리 예약
            }
            else {
                top--;                      // 오른쪽 없음: 이 노드 완료
            }
            break;

        case 3:                            // 오른쪽 자식 처리 완료 후
            pos++;                          // ')' 소비
            top--;                          // 이 노드 완료
            break;
        }
    }
}

// 왼쪽으로 누운 형태 출력 (오른쪽 서브트리 -> 자신 -> 왼쪽 서브트리 순)
void printSideways(int idx, int depth) {
    if (idx >= MAX_NODES || tree[idx] == 0) return;

    printSideways(2 * idx + 1, depth + 1);   // 오른쪽 서브트리 (위쪽)

    for (int i = 0; i < depth; i++) printf("   ");
    if (depth > 0) printf("+--");
    printf("%c\n", tree[idx]);

    printSideways(2 * idx, depth + 1);       // 왼쪽 서브트리 (아래쪽)
}

int main(void) {
    printf("이진트리를 괄호 형태로 입력하세요: ");
    scanf("%s", input);

    memset(tree, 0, sizeof(tree));
    pos = 0;
    buildTree(1);

    printf("\n[배열 표현] (인덱스 1이 루트, 왼쪽자식=2*i, 오른쪽자식=2*i+1)\n");
    for (int i = 1; i < MAX_NODES; i++) {
        if (tree[i] != 0)
            printf("tree[%d] = %c\n", i, tree[i]);
    }

    printf("\n[왼쪽으로 누운 형태 출력]\n");
    printSideways(1, 0);

    return 0;
}