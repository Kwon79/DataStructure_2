typedef struct Node
{
    char data;
    struct Node* left;
    struct Node* right;
} Node;


/* 노드 생성 */
Node* createNode(char data);

/* 트리 생성 */
Node* buildTree(const char* input);

/* 트리 출력 */
void printTree(Node* root);

/* 트리 정보 */
int countNodes(Node* root);
int countLeafNodes(Node* root);
int countNonLeafNodes(Node* root);
int getHeight(Node* root);
int getDegree(Node* root);

/* 트리 종류 */
int isComplete(Node* root);
int isFull(Node* root);
int isSkewed(Node* root);

/* 특정 노드 */
Node* findNode(Node* root, char data);
Node* findParent(Node* root, char data);
void printChildren(Node* root, char data);
void printSibling(Node* root, char data);

/* 메모리 해제 */
void freeTree(Node* root);