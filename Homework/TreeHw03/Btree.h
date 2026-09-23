typedef struct BNode
{
    char data;
    struct BNode* left;
    struct BNode* right;
} BNode;

typedef struct
{
    BNode* root;
    int size;
    int capacity;
} BTree;


/* 트리 생성 */
BTree* create_btree(int size);

/* 루트 삽입 */
int insert_root(BTree* tree, char value);

/* 자식 삽입 */
int insert_child(BTree* tree, const char* parent,
    char child, char value);

/* 단말 노드 삭제 */
int delete_node(BTree* tree, const char* leaf);

/* 노드 데이터 수정 */
int update_value(BTree* tree, const char* node, char value);

/* 자식 정보 출력 */
void read_child(BTree* tree, const char* parent);

/* 트리 출력 */
void print_btree(BTree* tree);

/* 트리 제거 */
void destroy_btree(BTree* tree);