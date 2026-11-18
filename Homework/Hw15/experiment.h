#pragma once
#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <stddef.h>

/* 삽입/삭제 연산 1개 (두 자료구조에 동일하게 적용) */
typedef struct {
    int is_insert;   /* 1: 삽입, 0: 삭제 */
    int value;       /* 삽입하거나 삭제할 값 */
} Operation;

/* 한 자료구조에 대한 실험 결과 */
typedef struct {
    double    time_ms;        /* 총 수행시간 (정렬 + 삽입/삭제) */
    long long comparisons;    /* 데이터 값 비교 횟수 */
    long long movements;      /* 데이터 값 복사/대입 횟수 */
    size_t    memory_bytes;   /* 최대 메모리 사용량 */
    int       final_count;    /* 모든 연산 후 데이터 개수 */
    int       ok;             /* 1: 정렬 상태 검증 통과 */
} ExpResult;

/*
 * origin : 원본 데이터 (n개, 수정하지 않음)
 * ops    : 삽입/삭제 연산 목록 (num_ops개)
 * out    : 최종 데이터를 오름차순으로 돌려받을 버퍼 (최소 n + num_ops개)
 */
ExpResult run_quick_experiment(const int* origin, int n, const Operation* ops, int num_ops, int* out);
ExpResult run_avl_experiment(const int* origin, int n, const Operation* ops, int num_ops, int* out);

/* 전체 실험: 데이터/연산 생성 -> 두 방법 실행 -> 결과 출력 */
void run_experiment(void);

#endif /* EXPERIMENT_H */