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

Node* insertBST(Node* root, int data,int *count) {
	if (root == NULL) return createTree(data);
	Node* cur = root;
	while (1) {
		(*count)++;
		if (data < cur->data) {
			if (cur->left == NULL) { cur->left = createTree(data); break; }
			cur = cur->left;
		}
		else {
			if (cur->right == NULL) {
				cur->right = createTree(data);
				break;
			}
			cur = cur->right;
		}
	}
	return root;
}

int searchBST(Node* root, int data, int* found) {
	int cnt = 0;
	*found = 0;
	Node* cur = root;
	while (cur != NULL) {
		cnt++;
		if (data == cur->data) { *found = 1;break; }
		else if (data < cur->data) cur = cur->left;
		else cur = cur->right;
	}
	return cnt;
}

int sequentialSearch(int* arr, int size, int data, int* found) {

}
void destroyTree(Node* root)