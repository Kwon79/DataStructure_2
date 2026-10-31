#ifndef SORT_H
#define SORT_H

/* ---------------------------------------------------------
 * 정렬 함수: 오름차순으로 정렬하고, 정렬 중 발생한
 * 데이터 값 사이의 비교 횟수를 반환한다.
 * --------------------------------------------------------- */
long long insertion_sort(int a[], int n);        /* [1] 삽입 정렬 */
long long shell_sort_original(int a[], int n);   /* [2] 쉘 정렬 - 기본 간격 (n/2, n/4, ..., 1) */
long long shell_sort_knuth(int a[], int n);      /* [3] 쉘 정렬 - Knuth 간격 (..., 13, 4, 1) */

/* 오름차순 정렬 여부 검증: 정렬되어 있으면 1, 아니면 0 */
int is_sorted(const int a[], int n);

/* 환경에 상관없이 동일한 데이터를 만들기 위한 난수 생성기 (xorshift32) */
void seed_random(unsigned int seed);
int  next_rand(void);

#endif /* SORT_H */