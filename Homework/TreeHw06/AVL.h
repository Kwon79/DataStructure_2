typedef struct BSTNode {
	int data;
	struct BSTNode* left;
	struct BSTNode* right;
}BSTNode;

typedef struct AVLNode {
	int data;
	struct AVLNode* left;
	struct AVLNode* right;
	int height;
}AVLNode;

//BST탐색 함수
BSTNode* createBST(int data);
BSTNode* insertBST(BSTNode* root, int data, int* count);
int searchBST(BSTNode* root, int data, int* found);
int getHeightBST(BSTNode* root);
void destroyBST(BSTNode* root);

// AVL 관련 함수
AVLNode* createAVL(int data);
AVLNode* insertAVL(AVLNode* root, int data, int* count);
int searchAVL(AVLNode* root, int data, int* found);

int getHeightAVL(AVLNode* root);
int getBalance(AVLNode* root);
void updateHeight(AVLNode* root);

AVLNode* rotateRight(AVLNode* root);
AVLNode* rotateLeft(AVLNode* root);

void destroyAVL(AVLNode* root);

//순차탐색 함수
int sequentialSearch(int* arr, int size, int data, int* found);
