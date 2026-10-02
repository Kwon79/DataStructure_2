#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#include"BST.h"

#define MAX_VALUE  1000
#define DATA_COUNT 100
#define KEY_COUNT  50

int main() {
	srand((unsigned)time(NULL));

	int arr[DATA_COUNT];
	int keys[KEY_COUNT];
	int used[MAX_VALUE + 1] = { 0 };
	Node* root = NULL;
	int buildCount = 0;

	for (int i = 0; i < DATA_COUNT;) {
		int v = rand() % (MAX_VALUE + 1);
		if (used[v]) continue;
		used[v] = 1;
		arr[i++] = v;
	}

	for (int i = 0;i < DATA_COUNT;i++) {
		root = insertBST(root, arr[i], &buildCount);
	}

	printf("===생성된 100개의 정수===\n");
	for (int i = 0; i < DATA_COUNT;i++) {
		printf("%4d", arr[i]);
		if (i % 10 == 9) {
			printf("\n");
		}
	}
	printf("\n BST 생성 비교 횟수: %d\n\n", buildCount);

	for (int i = 0;i < KEY_COUNT;i++) {
		keys[i] = rand() % (MAX_VALUE + 1);
	}

	int seqTotal = 0, bstTotal = 0;

	printf("%-4s %-6s %-10s %-8s %-8s\n", "No", "Key", "Result", "Seq", "BST");
	for (int i = 0;i < KEY_COUNT;i++) {
		int found;
		int seqCount = sequentialSearch(arr, DATA_COUNT, keys[i], &found);
		int bstCount = searchBST(root, keys[i], &found);

		char* result;
		if (found) {
			result = "Found";
		}
		else {
			result = "NOT Found";
		}
		printf("%-4d %-6d %-10s %-8d %-8d\n", i + 1, keys[i],
			result, seqCount, bstCount);

		seqTotal += seqCount;
		bstTotal += bstCount;
	}
	printf("\n[순차 탐색] 총 %d회, 평균 %.2f회\n", seqTotal, (double)seqTotal / KEY_COUNT);
	printf("[BST 탐색]  총 %d회, 평균 %.2f회\n", bstTotal, (double)bstTotal / KEY_COUNT);
	printf("[BST 생성 + 탐색] 총 %d회\n", buildCount + bstTotal);

	destroyTree(root);

	return 0;
}