#define BTREE_TRAVERSAL_H

typedef struct BNode
{
    char data;
    struct BNode* left;
    struct BNode* right;
} BNode;

/* 트리 생성 */
BNode* create_node(char data);

/* 괄호 표기법으로 트리 생성 */
BNode* build_tree(const char* input);

/* 트리 구조 출력 */
void print_tree(BNode* root);

/* 반복적 순회 */
void preorder(BNode* root);
void inorder(BNode* root);
void postorder(BNode* root);

/* 트리 메모리 해제 */
void destroy_tree(BNode* root);

