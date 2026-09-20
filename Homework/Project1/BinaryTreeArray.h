#define MAX 100

typedef struct
{
    char tree[MAX];
    int size;
} ArrayTree;


// 트리 초기화
void initTree(ArrayTree* t);

// 입력 문자열을 배열 이진트리로 변환
int buildTree(ArrayTree* t, const char* input);

// 트리 출력
void printTree(const ArrayTree* t);

// 트리 정보 출력
void printTreeInfo(const ArrayTree* t);

// 이진트리 종류 출력
void printTreeType(const ArrayTree* t);

// 특정 노드 찾기
int findNode(const ArrayTree* t, char data);

// 부모 출력
void printParent(const ArrayTree* t, char data);

// 자식 출력
void printChildren(const ArrayTree* t, char data);

// 형제 출력
void printSibling(const ArrayTree* t, char data);