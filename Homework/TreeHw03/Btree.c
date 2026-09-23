#include <stdio.h>
#include <stdlib.h>
#include "BTree.h"


/* 새로운 노드 생성 */
BNode* create_node(char data)
{
    BNode* newNode;

    newNode = (BNode*)malloc(sizeof(BNode));

    if (newNode == NULL)
        return NULL;

    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;
}


/* 경로를 이용하여 노드 찾기 */
BNode* find_node(BTree* tree, const char* path)
{
    BNode* current;
    int i;

    if (tree == NULL || tree->root == NULL)
        return NULL;

    if (path == NULL || path[0] != '/')
        return NULL;

    if (path[1] == '\0')
        return NULL;

    /* 첫 번째 노드는 반드시 루트 */
    if (path[1] != tree->root->data)
        return NULL;

    current = tree->root;
    i = 2;

    while (path[i] != '\0')
    {
        if (path[i] != '/')
            return NULL;

        i++;

        if (path[i] == '\0')
            return NULL;

        if (path[i] < 'A' || path[i] > 'Z')
            return NULL;

        /*
            같은 부모의 왼쪽과 오른쪽 자식은
            같은 데이터를 가질 수 없기 때문에
            데이터만으로 어느 자식인지 구분 가능
        */

        if (current->left != NULL &&
            current->left->data == path[i])
        {
            current = current->left;
        }
        else if (current->right != NULL &&
            current->right->data == path[i])
        {
            current = current->right;
        }
        else
        {
            return NULL;
        }

        i++;
    }

    return current;
}


/* 부모 노드 찾기 */
BNode* find_parent(BTree* tree, const char* path)
{
    BNode* current;
    int i;

    if (tree == NULL || tree->root == NULL)
        return NULL;

    if (path == NULL || path[0] != '/')
        return NULL;

    if (path[1] == '\0')
        return NULL;

    /* 루트 노드는 부모가 없음 */
    if (path[2] == '\0')
        return NULL;

    if (path[1] != tree->root->data)
        return NULL;

    current = tree->root;
    i = 2;

    while (path[i] != '\0')
    {
        if (path[i] != '/')
            return NULL;

        i++;

        if (path[i] == '\0')
            return NULL;

        /*
            현재 path[i]가 마지막 데이터라면
            current가 그 노드의 부모
        */
        if (path[i + 1] == '\0')
            return current;

        if (current->left != NULL &&
            current->left->data == path[i])
        {
            current = current->left;
        }
        else if (current->right != NULL &&
            current->right->data == path[i])
        {
            current = current->right;
        }
        else
        {
            return NULL;
        }

        i++;
    }

    return NULL;
}


/* 빈 이진트리 생성 */
BTree* create_btree(int size)
{
    BTree* tree;

    if (size <= 0)
        return NULL;

    tree = (BTree*)malloc(sizeof(BTree));

    if (tree == NULL)
        return NULL;

    tree->root = NULL;
    tree->size = 0;
    tree->capacity = size;

    return tree;
}


/* 루트 생성 */
int insert_root(BTree* tree, char value)
{
    if (tree == NULL)
        return 0;

    /* 이미 루트가 존재 */
    if (tree->root != NULL)
        return 0;

    if (tree->size >= tree->capacity)
        return 0;

    if (value < 'A' || value > 'Z')
        return 0;

    tree->root = create_node(value);

    if (tree->root == NULL)
        return 0;

    tree->size++;

    return 1;
}


/*
    자식 추가

    조건:
    1. 부모가 존재해야 함
    2. 부모의 자식은 최대 2개
    3. 지정한 L/R 위치가 비어 있어야 함
    4. 같은 부모의 다른 자식과 데이터가 같으면 안 됨
*/
int insert_child(BTree* tree, const char* parent,
    char child, char value)
{
    BNode* parentNode;
    BNode* newNode;

    if (tree == NULL || tree->root == NULL)
        return 0;

    if (child != 'L' && child != 'R')
        return 0;

    if (value < 'A' || value > 'Z')
        return 0;

    parentNode = find_node(tree, parent);

    if (parentNode == NULL)
        return 0;

    /*
        지정한 위치에 이미 자식이 있으면 추가 불가능
    */
    if (child == 'L' && parentNode->left != NULL)
        return 0;

    if (child == 'R' && parentNode->right != NULL)
        return 0;

    /*
        반대쪽 자식과 같은 데이터 사용 불가
    */
    if (child == 'L')
    {
        if (parentNode->right != NULL &&
            parentNode->right->data == value)
            return 0;
    }
    else
    {
        if (parentNode->left != NULL &&
            parentNode->left->data == value)
            return 0;
    }

    if (tree->size >= tree->capacity)
        return 0;

    newNode = create_node(value);

    if (newNode == NULL)
        return 0;

    if (child == 'L')
        parentNode->left = newNode;
    else
        parentNode->right = newNode;

    tree->size++;

    return 1;
}


/* 단말 노드 삭제 */
int delete_node(BTree* tree, const char* path)
{
    BNode* target;
    BNode* parent;

    if (tree == NULL || tree->root == NULL)
        return 0;

    target = find_node(tree, path);

    if (target == NULL)
        return 0;

    /*
        단말 노드인지 확인
    */
    if (target->left != NULL ||
        target->right != NULL)
        return 0;

    /*
        루트가 단말 노드인 경우
    */
    if (target == tree->root)
    {
        free(target);

        tree->root = NULL;
        tree->size--;

        return 1;
    }

    parent = find_parent(tree, path);

    if (parent == NULL)
        return 0;

    if (parent->left == target)
    {
        parent->left = NULL;
    }
    else if (parent->right == target)
    {
        parent->right = NULL;
    }
    else
    {
        return 0;
    }

    free(target);
    tree->size--;

    return 1;
}


/* 노드 데이터 수정 */
int update_value(BTree* tree, const char* path, char value)
{
    BNode* target;
    BNode* parent;

    if (tree == NULL || tree->root == NULL)
        return 0;

    if (value < 'A' || value > 'Z')
        return 0;

    target = find_node(tree, path);

    if (target == NULL)
        return 0;

    /*
        루트는 부모가 없으므로
        다른 자식과의 중복 검사 필요 없음
    */
    if (target == tree->root)
    {
        target->data = value;
        return 1;
    }

    parent = find_parent(tree, path);

    if (parent == NULL)
        return 0;

    /*
        같은 부모의 다른 자식과
        같은 데이터가 되는지 검사
    */
    if (parent->left == target)
    {
        if (parent->right != NULL &&
            parent->right->data == value)
            return 0;
    }
    else if (parent->right == target)
    {
        if (parent->left != NULL &&
            parent->left->data == value)
            return 0;
    }
    else
    {
        return 0;
    }

    target->data = value;

    return 1;
}


/* 자식 정보 출력 */
void read_child(BTree* tree, const char* parent)
{
    BNode* node;
    int printed = 0;

    if (tree == NULL || tree->root == NULL)
    {
        printf("트리가 비어 있습니다.\n");
        return;
    }

    node = find_node(tree, parent);

    if (node == NULL)
    {
        printf("존재하지 않는 노드 경로입니다.\n");
        return;
    }

    if (node->left != NULL)
    {
        printf("%c(L)", node->left->data);
        printed = 1;
    }

    if (node->right != NULL)
    {
        if (printed)
            printf(", ");

        printf("%c(R)", node->right->data);
        printed = 1;
    }

    if (!printed)
        printf("자식 노드가 없습니다.");

    printf("\n");
}


/* 트리 출력 */
void print_node(BNode* node, int depth, int isRoot)
{
    int i;

    if (node == NULL)
        return;

    if (isRoot)
    {
        printf("%c\n", node->data);
    }
    else
    {
        for (i = 0; i < depth; i++)
            printf("    ");

        printf("+---%c\n", node->data);
    }

    print_node(node->left, depth + 1, 0);
    print_node(node->right, depth + 1, 0);
}


void print_btree(BTree* tree)
{
    if (tree == NULL || tree->root == NULL)
    {
        printf("트리가 비어 있습니다.\n");
        return;
    }

    print_node(tree->root, 0, 1);
}


/* 노드 전체 삭제 */
void destroy_node(BNode* node)
{
    if (node == NULL)
        return;

    destroy_node(node->left);
    destroy_node(node->right);

    free(node);
}


/* 트리 삭제 */
void destroy_btree(BTree* tree)
{
    if (tree == NULL)
        return;

    destroy_node(tree->root);

    free(tree);
}