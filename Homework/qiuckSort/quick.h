#ifndef QUICKSORT_H
#define QUICKSORT_H

/* 피벗 선택 방법 */
typedef enum {
    PIVOT_FIRST,    /* 첫 번째 원소 */
    PIVOT_RANDOM,   /* 무작위 원소 */
    PIVOT_MEDIAN3   /* 첫/가운데/마지막 중앙값 */
} PivotType;

/* 무작위 정수 데이터 생성 (중복 허용) */
void generate_data(int arr[], int n);

/* 재귀적 퀵 정렬. cmp에 숫자 비교 횟수를 누적한다. */
void quick_sort_recursive(int arr[], int n, PivotType type, long long* cmp);

/* 스택을 이용한 반복적 퀵 정렬. 피벗 선택/분할은 재귀 버전과 동일. */
void quick_sort_iterative(int arr[], int n, PivotType type, long long* cmp);

/* 오름차순 정렬 여부 확인 (정렬되어 있으면 1, 아니면 0) */
int is_sorted(const int arr[], int n);

/* 원본 복사본을 재귀 퀵 정렬하고 비교 횟수를 반환 (검증 실패 시 -1) */
long long count_comparisons(const int origin[], int n, PivotType type);

/* 재귀/반복 퀵 정렬의 평균 실행시간(ms) 측정.
   데이터 복사, 검증 시간은 제외하고 정렬 구간만 측정한다. */
void measure_time(const int origin[], int n, PivotType type, int repeat,
    double* rec_ms, double* iter_ms);

#endif /* QUICKSORT_H */