#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "BTree.h"


/* 명령어 확인 */
int is_command(const char* command, char shortCommand)
{
    if (command == NULL)
        return 0;

    /*
        I 또는 Insert
        D 또는 Delete
        U 또는 Update
        R 또는 Read
        P 또는 Print
    */

    if (strlen(command) == 1)
    {
        return toupper(command[0]) == shortCommand;
    }

    return toupper(command[0]) == shortCommand;
}


/* 영문 대문자 한 글자인지 확인 */
int is_upper_char(const char* str)
{
    if (str == NULL)
        return 0;

    if (strlen(str) != 1)
        return 0;

    if (str[0] < 'A' || str[0] > 'Z')
        return 0;

    return 1;
}


/* 경로 형식 확인 */
int is_valid_path(const char* path)
{
    int i;

    if (path == NULL)
        return 0;

    if (path[0] != '/')
        return 0;

    /* "/" 자체는 빈 트리의 루트 생성에서 사용 */
    if (path[1] == '\0')
        return 1;

    if (path[1] < 'A' || path[1] > 'Z')
        return 0;

    i = 2;

    while (path[i] != '\0')
    {
        if (path[i] != '/')
            return 0;

        i++;

        if (path[i] == '\0')
            return 0;

        if (path[i] < 'A' || path[i] > 'Z')
            return 0;

        i++;
    }

    return 1;
}


int main(void)
{
    BTree* tree;
    char input[200];

    /*
        트리 자체도 필요한 만큼 동적으로 생성
    */
    tree = create_btree();

    if (tree == NULL)
    {
        printf("트리 생성에 실패했습니다.\n");
        return 1;
    }


    /* 명령어 안내 */
    printf("====================================\n");
    printf("       이진트리 조작 프로그램\n");
    printf("====================================\n");
    printf("사용 가능한 명령어\n");
    printf("------------------------------------\n");
    printf("Insert / A       : 루트 노드 생성\n");
    printf("Insert /A L B    : 왼쪽 자식 B 추가\n");
    printf("Insert /A R B    : 오른쪽 자식 B 추가\n");
    printf("Delete /A/B      : 단말 노드 삭제\n");
    printf("Update /A/B C    : 노드 데이터 수정\n");
    printf("Read /A/B        : 자식 노드 확인\n");
    printf("Print            : 전체 트리 출력\n");
    printf("------------------------------------\n");
    printf("※ 명령어는 첫 글자만 입력해도 됩니다.\n");
    printf("  예: I /A L B, D /A/B, P\n");
    printf("====================================\n\n");


    while (1)
    {
        char* command;
        char* arg1;
        char* arg2;
        char* arg3;
        char* extra;

        printf("> ");

        if (fgets(input, sizeof(input), stdin) == NULL)
            break;

        input[strcspn(input, "\n")] = '\0';

        if (strlen(input) == 0)
            continue;


        command = strtok(input, " \t");
        arg1 = strtok(NULL, " \t");
        arg2 = strtok(NULL, " \t");
        arg3 = strtok(NULL, " \t");
        extra = strtok(NULL, " \t");


        /* =========================
           INSERT
           ========================= */

        if (is_command(command, 'I'))
        {
            /*
                Insert / A
                루트 생성
            */
            if (arg1 != NULL &&
                strcmp(arg1, "/") == 0)
            {
                if (arg2 == NULL || extra != NULL)
                {
                    printf("오류: Insert 명령의 인자가 올바르지 않습니다.\n");
                    continue;
                }

                if (!is_upper_char(arg2))
                {
                    printf("오류: 노드 데이터는 영문 대문자 한 글자여야 합니다.\n");
                    continue;
                }

                if (tree->root != NULL)
                {
                    printf("오류: 이미 루트 노드가 존재합니다.\n");
                    continue;
                }

                if (insert_root(tree, arg2[0]))
                {
                    printf("루트 노드 %c가 생성되었습니다.\n",
                        arg2[0]);
                }
                else
                {
                    printf("오류: 루트 노드를 생성할 수 없습니다.\n");
                }

                continue;
            }


            /*
                Insert /A L B
                자식 생성
            */

            if (arg1 == NULL ||
                arg2 == NULL ||
                arg3 == NULL ||
                extra != NULL)
            {
                printf("오류: Insert 명령의 인자가 올바르지 않습니다.\n");
                continue;
            }

            if (!is_valid_path(arg1) ||
                strcmp(arg1, "/") == 0)
            {
                printf("오류: 잘못된 부모 노드 경로입니다.\n");
                continue;
            }

            if (strlen(arg2) != 1)
            {
                printf("오류: 자식 위치는 L 또는 R이어야 합니다.\n");
                continue;
            }

            arg2[0] = toupper(arg2[0]);

            if (arg2[0] != 'L' &&
                arg2[0] != 'R')
            {
                printf("오류: 자식 위치는 L 또는 R이어야 합니다.\n");
                continue;
            }

            if (!is_upper_char(arg3))
            {
                printf("오류: 노드 데이터는 영문 대문자 한 글자여야 합니다.\n");
                continue;
            }

            if (insert_child(tree,
                arg1,
                arg2[0],
                arg3[0]))
            {
                printf("노드 %c가 추가되었습니다.\n",
                    arg3[0]);
            }
            else
            {
                printf("오류: 노드를 추가할 수 없습니다.\n");
            }

            continue;
        }


        /* =========================
           DELETE
           ========================= */

        if (is_command(command, 'D'))
        {
            if (arg1 == NULL ||
                extra != NULL)
            {
                printf("오류: Delete 명령의 인자가 올바르지 않습니다.\n");
                continue;
            }

            if (!is_valid_path(arg1) ||
                strcmp(arg1, "/") == 0)
            {
                printf("오류: 잘못된 노드 경로입니다.\n");
                continue;
            }

            if (delete_node(tree, arg1))
            {
                printf("노드가 삭제되었습니다.\n");
            }
            else
            {
                printf("오류: 존재하지 않거나 단말 노드가 아닌 노드입니다.\n");
            }

            continue;
        }


        /* =========================
           UPDATE
           ========================= */

        if (is_command(command, 'U'))
        {
            if (arg1 == NULL ||
                arg2 == NULL ||
                extra != NULL)
            {
                printf("오류: Update 명령의 인자가 올바르지 않습니다.\n");
                continue;
            }

            if (!is_valid_path(arg1) ||
                strcmp(arg1, "/") == 0)
            {
                printf("오류: 잘못된 노드 경로입니다.\n");
                continue;
            }

            if (!is_upper_char(arg2))
            {
                printf("오류: 변경할 데이터는 영문 대문자 한 글자여야 합니다.\n");
                continue;
            }

            if (update_value(tree, arg1, arg2[0]))
            {
                printf("노드 데이터가 수정되었습니다.\n");
            }
            else
            {
                printf("오류: 노드를 수정할 수 없습니다.\n");
            }

            continue;
        }


        /* =========================
           READ
           ========================= */

        if (is_command(command, 'R'))
        {
            if (arg1 == NULL ||
                extra != NULL)
            {
                printf("오류: Read 명령의 인자가 올바르지 않습니다.\n");
                continue;
            }

            if (!is_valid_path(arg1) ||
                strcmp(arg1, "/") == 0)
            {
                printf("오류: 잘못된 노드 경로입니다.\n");
                continue;
            }

            read_child(tree, arg1);

            continue;
        }


        /* =========================
           PRINT
           ========================= */

        if (is_command(command, 'P'))
        {
            if (arg1 != NULL)
            {
                printf("오류: Print 명령은 추가 인자가 필요하지 않습니다.\n");
                continue;
            }

            print_btree(tree);

            continue;
        }


        /* =========================
           잘못된 명령
           ========================= */

        printf("오류: 잘못된 명령어입니다.\n");
    }


    /* 프로그램 종료 전 모든 메모리 해제 */
    destroy_btree(tree);

    return 0;
}