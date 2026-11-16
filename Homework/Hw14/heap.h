#pragma once
#ifndef SORT_H
#define SORT_H

#include <stddef.h>

/* 한 번의 정렬 실행에서 측정한 결과 */
typedef struct {
    long long comparisons;   /* 숫자 비교 횟수 */
    double    time_ms;       /* 실행시간 (ms) */
    size_t    memory_bytes;  /* 최대 추가 메모리 (Byte) */
    int       sorted_ok;     /* 1: 오름차순 정렬 확인됨 */
} SortResult;

/* 각 정렬 알고리즘 (a: 정렬할 배열, n: 원소 수) */
SortResult heap_sort(int* a, int n);
SortResult merge_sort(int* a, int n);
SortResult quick_sort_first(int* a, int n);
SortResult quick_sort_random(int* a, int n);
SortResult quick_sort_median3(int* a, int n);

/* 실험 전체 실행: 데이터 생성 -> 5가지 정렬 -> 결과 출력 */
void run_experiment(void);

#endif /* SORT_H */