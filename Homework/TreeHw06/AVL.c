#include<stdio.h>
#include<stdlib.h>

#include"AVL.h"

//BST탐색 함수
BSTNode* createBST(int data) {
	BSTNode* BST = (BSTNode*)malloc(sizeof(BSTNode));
	BST->data = data;
	BST->left = NULL;
	BST->right = NULL;

	return BST;
}

BSTNode* insertBST(BSTNode* root, int data, int* count) {
	if (root == NULL) return createBST(data);
	(*count)++;
	if (root->data == data) {
		
	}
	else if (root->data > data) {
		root->left = insertBST(root->left, data,count);
	}
	else {
		root->right = insertBST(root->right, data, count);
	}

	return root;
}

int searchBST(BSTNode* root, int data, int* found) {
	if (root == NULL) {
		*found = 0;
		return 0;
	}

	if (data == root->data) {
		*found = 1;
		return 1;
	}

	if (data < root->data) {
		return 1 + searchBST(root->left, data, found);
	}
	else {
		return 1 + searchBST(root->right, data, found);
	}
}

int getHeightBST(BSTNode* root) {
	if (root == NULL) {
		return 0;
	}
	int left = getHeightBST(root->left);
	int right = getHeightBST(root->right);

	if (left > right) {
		return left + 1;
	}
	else {
		return right + 1;
	}
}

void destroyBST(BSTNode* root) {
	if (!root) return;
	destroyBST(root->left);
	destroyBST(root->right);
	free(root);
}

// AVL 관련 함수
AVLNode* createAVL(int data) {
	AVLNode* AVL = (AVLNode*)malloc(sizeof(AVLNode));

	AVL->data = data;
	AVL->left = NULL;
	AVL->right = NULL;
	AVL->height = 1;

	return AVL;
}

AVLNode* insertAVL(AVLNode* root, int data, int* count) {
	if (root == NULL)return createAVL(data);
	(*count)++;
	
	if (root->data == data) {

	}
	else if (root->data < data) {
		root->right = insertAVL(root->right, data, count);
		updateHeight(root);
		int balance = getBalance(root);
		if (balance < -1) {
			if (root->right->data > data) {
				root->right = rotateRight(root->right);
				root = rotateLeft(root);
			}
			else {
				root = rotateLeft(root);
			}
		}
	}
	else {
		root->left = insertAVL(root->left, data, count);
		updateHeight(root);
		int balance = getBalance(root);
		if (balance > 1) {
			if (root->left->data < data) {
				root->left = rotateLeft(root->left);
				root = rotateRight(root);
			}
			else {
				root = rotateRight(root);
			}
		}
	}
	return root;
}

int searchAVL(AVLNode* root, int data, int* found) {
	if (root == NULL) {
		*found = 0;
		return 0;
	}
	if (root->data == data) {
		*found = 1;
		return 1;
	}
	if (root->data < data) {
		return 1 + searchAVL(root->right, data, found);
	}
	else
	{
		return 1 + searchAVL(root->left, data, found);
	}
}

int getHeightAVL(AVLNode* root) {
	if (root == NULL) return 0;

	int left = getHeightAVL(root->left);
	int right = getHeightAVL(root->right);

	if (left > right) {
		return left + 1;
	}
	else {
		return right + 1;
	}
}

int getBalance(AVLNode* root) {
	if (root == NULL) return 0;

	int left = 0;
	int right = 0;

	if (root->left != NULL) {
		left = root->left->height;
	}
	if (root->right != NULL) {
		right = root->right->height;
	}

	return left - right;
}

void updateHeight(AVLNode* root) {
	int left = 0;
	int right = 0;

	if (root->left != NULL) {
		left = root->left->height;
	}

	if (root->right != NULL) {
		right = root->right->height;
	}

	if (left > right) {
		root->height = left + 1;
	}
	else {
		root->height = right + 1;
	}
}

AVLNode* rotateRight(AVLNode* root) {
	AVLNode* temp = root->left;
	AVLNode* child = temp->right;
	root->left = child;
	temp->right = root;

	updateHeight(root);
	updateHeight(temp);

	return temp;
}

AVLNode* rotateLeft(AVLNode* root) {
	AVLNode* temp = root->right;
	AVLNode* child = temp->left;
	root->right = child;
	temp->left = root;

	updateHeight(root);
	updateHeight(temp);

	return temp;
}

void destroyAVL(AVLNode* root) {
	if (!root)return;
	destroyAVL(root->left);
	destroyAVL(root->right);
	free(root);
}

//순차탐색 함수
int sequentialSearch(int* arr, int size, int data, int* found) {
	int cnt=0;
	*found = 0;
	for (int i = 0;i < size;i++) {
		cnt++;
		if (arr[i] == data) { *found = 1;break; }
	}
	return cnt;

}
