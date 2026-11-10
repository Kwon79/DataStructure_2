#ifndef SORTS_H
#define SORTS_H

/* 정렬 알고리즘 종류 */
typedef enum {
    SORT_MERGE,          /* 병합 정렬 */
    SORT_QUICK_FIRST,    /* 퀵 정렬 - 첫 번째 원소 피벗 */
    SORT_QUICK_RANDOM,   /* 퀵 정렬 - 무작위 원소 피벗 */
    SORT_QUICK_MEDIAN3   /* 퀵 정렬 - 첫/가운데/마지막 중앙값 피벗 */
} SortType;

/* 정렬 1종류를 데이터 1개에 적용한 측정 결과 */
typedef struct {
    long long comparisons;  /* 숫자 비교 횟수 (정렬 결과가 틀리면 -1) */
    double    time_ms;      /* 실행시간 평균 (ms) */
    long long memory;       /* 최대 추가 메모리 (Byte) */
} SortStat;

/* 무작위 정수 데이터 생성 (중복 허용) */
void generate_data(int arr[], int n);

/* 오름차순 정렬 여부 확인 (정렬되어 있으면 1, 아니면 0) */
int is_sorted(const int arr[], int n);

/* 정렬 이름 문자열 (출력용) */
const char* sort_name(SortType type);

/* origin의 복사본을 type 방식으로 정렬하며 성능을 측정한다.
   - 비교 횟수, 메모리: 첫 번째 실행의 값
   - 실행시간: repeat번 실행한 평균 (배열 복사, 검증 시간은 제외) */
SortStat measure_sort(const int origin[], int n, SortType type, int repeat);

#endif /* SORTS_H */