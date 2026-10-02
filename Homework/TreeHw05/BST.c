#include<stdio.h>
#include<stdlib.h>

#include"BST.h"

Node* createTree(int data) {
	Node* tree = (Node*)malloc(sizeof(Node));
	tree->data = data;
	tree->left = NULL;
	tree->right = NULL;

	return tree;
}

Node* insertBST(Node* root, int data, int* count) {
	if (root == NULL) return createTree(data);
	(*count)++;                                   // 기존 노드와 비교할 때마다 +1
	if (data < root->data)
		root->left = insertBST(root->left, data, count);
	else
		root->right = insertBST(root->right, data, count);
	return root;
}

int searchBST(Node* root, int data, int* found) {
	if (root == NULL) {          // 더 내려갈 노드 없음 -> 실패
		*found = 0;
		return 0;
	}
	if (data == root->data) {    // 찾음 -> 성공
		*found = 1;
		return 1;
	}
	if (data < root->data)
		return 1 + searchBST(root->left, data, found);
	return 1 + searchBST(root->right, data, found);
}

int sequentialSearch(int* arr, int size, int data, int* found) {
	int cnt = 0;
	*found = 0;
	for (int i = 0;i < size;i++) {
		cnt++;
		if (arr[i] == data) { *found = 1;break; }

	}
	return cnt;
}
void destroyTree(Node* root) {
	if (!root)return;
	destroyTree(root->left);
	destroyTree(root->right);
	free(root);
}