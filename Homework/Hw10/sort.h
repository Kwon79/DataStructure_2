#pragma once
#ifndef SORT_H
#define SORT_H

/* ---------- 상수 / 자료형 ---------- */
#define N          10000      /* 데이터 개수 */
#define TRIALS     100        /* 무작위 반복 횟수 */
#define MAX_KEY    1000000    /* 키의 최댓값 */

typedef struct {
    int key1;   /* 정렬 기준 키 */
    int key2;   /* 정렬에 사용하지 않는 두 번째 키 */
    int order;  /* 원래 입력 순번 (안정성 검증용, 정렬 기준 아님) */
} Data;

typedef void (*SortFn)(Data*, int);

/* ---------- 카운터 ---------- */
extern unsigned long long g_cmp;   /* key1 비교 횟수 */
extern unsigned long long g_mov;   /* 데이터 이동 횟수 */
void reset_counters(void);

/* 데이터 1개를 통째로 복사하면 이동 1회 */
#define MOVE(dst, src) do { (dst) = (src); g_mov++; } while (0)

/* ---------- 정렬 알고리즘 ---------- */
void selection_sort(Data* a, int n);
void bubble_sort(Data* a, int n);          /* 모든 순회 수행 */
void bubble_sort_early(Data* a, int n);    /* 교환 없으면 조기 종료 */
void insertion_sort_gt(Data* a, int n);    /* Case A: >  (안정) */
void insertion_sort_ge(Data* a, int n);    /* Case B: >= (불안정) */

/* ---------- 유틸 ---------- */
void         rng_seed(unsigned int s);
unsigned int rng_next(void);
int          rand_key(void);                 /* 0 ~ MAX_KEY */
void         make_random(Data* a, int n);
int          is_sorted(const Data* a, int n);
int          stability_violations(const Data* a, int n);
int          duplicate_pairs(const Data* a, int n);
int          cmp_asc(const void* p, const void* q);   /* 시험 데이터 준비용 qsort 비교 함수 */

/* ---------- 전체 실험 수행 및 결과 출력 ---------- */
void run_all_experiments(void);

#endif