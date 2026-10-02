typedef struct Node {
	int data;
	struct Node* left;
	struct Node* right;
}Node;

Node* createTree(int data);
Node* insertBST(Node* root, int data,int *count);
int searchBST(Node* root, int data,int *found);
int sequentialSearch(int *arr,int size,int data,int *found);
void destroyTree(Node* root);