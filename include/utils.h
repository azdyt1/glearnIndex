#ifndef __H_SYS_UTILS
#define __H_SYS_UTILS

#pragma once
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

#if defined(__AVX2__) ||                                                       \
    defined(__FMA__) // icpc, gcc and clang register __FMA__, VS does not
#define VECTORIZE 1
#define AVX_LOOP _Pragma("omp simd")
#else
#define AVX_LOOP // empty
#endif

#ifndef UINT64_C // VS can not detect the ##ULL macro
#define UINT64_C(c) (c##ULL)
#endif
#define SIGMOID_BOUND 6.0  // computation range for fast sigmoid lookup table
// #define DEFAULT_ALIGN 128  // default align in bytes; abandoned
#define MAX_CODE_LENGTH 64 // maximum HSM code length. sufficient for nv < int64
#define DEFAULT_ALIGN 128

int argPos(char *str, int argc, char **argv);

class myrandom {
public:
// http://xoroshiro.di.unimi.it/#shootout
// We use xoroshiro128+, the fastest generator available
    uint64_t rng_seed0, rng_seed1;

    myrandom(uint64_t seed);

    void reinit(uint64_t seed);

    static inline uint64_t rotl(const uint64_t x, int k) {
        return (x << k) | (x >> (64 - k));
    }

    uint64_t lrand();

    double drand();

    int irand(int max) { return lrand() % max; }

    int irand(int min, int max) { return lrand() % (max - min) + min; }
};

static myrandom mainrandom(time(nullptr));

const int sigmoid_table_size = 1024; // This should fit in L1 cache
const float SIGMOID_RESOLUTION = sigmoid_table_size / (SIGMOID_BOUND * 2.0f);
static float *sigmoid_table;

void init_sigmoid_table();

float fast_sigmoid(double x);

bool eq(float a, float b);

#endif 