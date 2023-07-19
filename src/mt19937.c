/* mt19937.c
 * Implementation of the Mersenne Twister MT19937 and MT19937-64 pseudorandom
 * number generators (PRNGs) in ANSI C.
 * Author: Dénes Fintha
 * Year: 2023
 * -------------------------------------------------------------------------- */

/* Interface */

#include <stdint.h>

#define MT19937_N 624
#define MT19937_64_N 312

typedef struct {
    uint32_t mt[MT19937_N];
    uint32_t index;
} mt19937_state;

typedef struct {
    uint64_t mt[MT19937_64_N];
    uint64_t index;
} mt19937_64_state;

void mt19937_seed(mt19937_state *state, uint32_t seed);
uint32_t mt19937_next(mt19937_state *state);
void mt19937_64_seed(mt19937_64_state *state, uint64_t seed);
uint64_t mt19937_64_next(mt19937_64_state *state);

/* Implementation */

#define MT19937_M 397
#define MT19937_W 32
#define MT19937_F 1812433253
#define MT19937_R 31
#define MT19937_A 0x9908B0DF
#define MT19937_U 11
#define MT19937_D 0xFFFFFFFF
#define MT19937_S 7
#define MT19937_B 0x9D2C5680
#define MT19937_T 15
#define MT19937_C 0xEFC60000
#define MT19937_L 18
#define MT19937_LOMASK ((((uint32_t) 1) << MT19937_R) - 1)
#define MT19937_HIMASK (~(MT19937_LOMASK))

#define MT19937_64_M 156
#define MT19937_64_W 64
#define MT19937_64_F 6364136223846793005
#define MT19937_64_R 31
#define MT19937_64_A 0xB5026F5AA96619E9
#define MT19937_64_U 29
#define MT19937_64_D 0x5555555555555555
#define MT19937_64_S 17
#define MT19937_64_B 0x71D67FFFEDA60000
#define MT19937_64_T 37
#define MT19937_64_C 0xFFF7EEE000000000
#define MT19937_64_L 43
#define MT19937_64_LOMASK ((((uint64_t) 1) << MT19937_64_R) - 1)
#define MT19937_64_HIMASK (~(MT19937_64_LOMASK))

void mt19937_seed(mt19937_state *state, uint32_t seed) {
    uint32_t i;
    state->index = MT19937_N;
    state->mt[0] = seed;
    for (i = 1; i < MT19937_N; ++i) {
        state->mt[i] =
            MT19937_F
            * (state->mt[i - 1] ^ (state->mt[i - 1] >> (MT19937_W - 2)))
            + i;
    }
}

static void mt19937_twist(mt19937_state *state) {
    uint32_t i, x, y;
    for (i = 0; i < MT19937_N; ++i) {
        x = state->mt[i] & MT19937_HIMASK;
        x |= state->mt[(i + 1) % MT19937_N] & MT19937_LOMASK;
        y = x >> 1;
        if (x % 2 != 0)
            y ^= MT19937_A;
        state->mt[i] = state->mt[(i + MT19937_M) % MT19937_N] ^ y;
    }
    state->index = 0;
}

uint32_t mt19937_next(mt19937_state *state) {
    uint32_t y;
    if (state->index >= MT19937_N) {
        if (state->index > MT19937_N)
            mt19937_seed(state, 5489);
        mt19937_twist(state);
    }
    y = state->mt[state->index];
    y ^= ((y >> MT19937_U) & MT19937_D);
    y ^= ((y << MT19937_S) & MT19937_B);
    y ^= ((y << MT19937_T) & MT19937_C);
    y ^= (y >> MT19937_L);
    ++state->index;
    return y;
}

void mt19937_64_seed(mt19937_64_state *state, uint64_t seed) {
    uint64_t i;
    state->index = MT19937_64_N;
    state->mt[0] = seed;
    for (i = 1; i < MT19937_64_N; ++i) {
        state->mt[i] =
            MT19937_64_F
            * (state->mt[i - 1] ^ (state->mt[i - 1] >> (MT19937_64_W - 2)))
            + i;
    }
}

static void mt19937_64_twist(mt19937_64_state *state) {
    uint64_t i, x, y;
    for (i = 0; i < MT19937_64_N; ++i) {
        x = state->mt[i] & MT19937_64_HIMASK;
        x |= state->mt[(i + 1) % MT19937_64_N] & MT19937_64_LOMASK;
        y = x >> 1;
        if (x % 2 != 0)
            y ^= MT19937_64_A;
        state->mt[i] = state->mt[(i + MT19937_64_M) % MT19937_64_N] ^ y;
    }
    state->index = 0;
}

uint64_t mt19937_64_next(mt19937_64_state *state) {
    uint64_t y;
    if (state->index >= MT19937_64_N) {
        if (state->index > MT19937_64_N)
            mt19937_64_seed(state, 5489);
        mt19937_64_twist(state);
    }
    y = state->mt[state->index];
    y ^= ((y >> MT19937_64_U) & MT19937_64_D);
    y ^= ((y << MT19937_64_S) & MT19937_64_B);
    y ^= ((y << MT19937_64_T) & MT19937_64_C);
    y ^= (y >> MT19937_64_L);
    ++state->index;
    return y;
}

/* Demonstration */

#include <inttypes.h>
#include <stdio.h>

int main(void) {
    int i;
    mt19937_state state;
    mt19937_64_state state_64;

    mt19937_seed(&state, 19940720);
    puts("Generating 10 random numbers with MT19937 using seed 19940720.\n");
    for (i = 0; i < 10; ++i) {
        printf("%" PRIu32 "\n", mt19937_next(&state));
    }

    putc('\n', stdout);

    mt19937_64_seed(&state_64, 19940720);
    puts("Generating 10 random numbers with MT19937-64 using seed 19940720.\n");
    for (i = 0; i < 10; ++i) {
        printf("%" PRIu64 "\n", mt19937_64_next(&state_64));
    }

    putc('\n', stdout);

    puts("The above results were validated using the std::mt19937 and ");
    puts("std::mt19937_64 classes from the C++ standard library (C++11 and");
    puts("above).");
    return 0;
}
