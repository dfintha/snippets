// mersenne_twister.cpp
// Implementation of the Mersenne Twister pseudorandom number generator (PRNG)
// engine family in C++11. The implementation interface is the same as the
// interface of std::mersenne_twister_engine.
// Author: Dénes Fintha
// Year: 2023
// -------------------------------------------------------------------------- //

// Interface

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iosfwd>

namespace my {
    template <typename UIntT,
              size_t W, size_t N, size_t M, size_t R,
              UIntT A, UIntT U, UIntT D, UIntT S,
              UIntT B, UIntT T, UIntT C, UIntT L,
              UIntT F>
    class mersenne_twister_engine {
    public:
        using result_type = UIntT;

    public:
        static constexpr size_t word_size = W;
        static constexpr size_t state_size = N;
        static constexpr size_t shift_size = M;
        static constexpr size_t mask_bits = R;
        static constexpr result_type xor_mask = A;
        static constexpr size_t tempering_u = U;
        static constexpr result_type tempering_d = D;
        static constexpr size_t tempering_s = S;
        static constexpr result_type tempering_b = B;
        static constexpr size_t tempering_t = T;
        static constexpr result_type tempering_c = C;
        static constexpr size_t tempering_l = L;
        static constexpr result_type initialization_multiplier = F;
        static constexpr result_type default_seed = 5489;

    public:
        mersenne_twister_engine() {
            seed(default_seed);
        }

        explicit mersenne_twister_engine(result_type value) {
            seed(value);
        }

        template <typename SeedSequence>
        explicit mersenne_twister_engine(SeedSequence& generator) {
            constexpr size_t u32_per_word = (word_size + 31) / 32;
            uint32_t seed_source[state_size * u32_per_word];
            generator.generate(seed_source, seed_source + N * u32_per_word);

            bool zero = true;
            for (size_t i = 0; i < state_size; ++i) {
                UIntT shift = 1;
                UIntT sum = 0;
                for (size_t j = 0; j < u32_per_word; ++j) {
                    sum += seed_source[u32_per_word * i + j] * shift;
                    if (word_size > 32)
                        shift <<= 32;
                }
                mt[i] = sum;
                if (zero && ((i == 0 && (mt[0] >> R) != 0) || mt[i] != 0))
                    zero = false;
            }

            if (zero)
                mt[0] = UIntT(1) << (W - 1);
            index = state_size;
        }

        mersenne_twister_engine(const mersenne_twister_engine&) = default;

        void seed(result_type value) {
            index = N;
            mt[0] = value;
            for (UIntT i = 1; i < N; ++i) {
                mt[i] = F * (mt[i - 1] ^ (mt[i - 1] >> (W - 2))) + i;
            }
        }

        result_type operator()() {
            return next();
        }

        void discard(unsigned long long times) {
            for (unsigned long long i = 0; i < times; ++i) {
                (void) next();
            }
        }

        static constexpr result_type min() {
            return 0U;
        }

        static constexpr result_type max() {
            return UIntT(-1);
        }

        friend bool operator==(
            const mersenne_twister_engine& lhs,
            const mersenne_twister_engine& rhs
        ) {
            return memcmp(&lhs, &rhs, sizeof(mersenne_twister_engine)) == 0;
        }

        friend bool operator!=(
            const mersenne_twister_engine& lhs,
            const mersenne_twister_engine& rhs
        ) {
            return !(lhs == rhs);
        }

        template <typename CharT, typename Traits>
        friend std::basic_ostream<CharT, Traits>& operator<<(
            std::basic_ostream<CharT, Traits>& stream,
            const mersenne_twister_engine& engine
        ) {
            for (size_t i = 0; i < N; ++i) {
                stream << engine.mt[i] << ' ';
            }
            stream << engine.index;
            return stream;
        }

        template <typename CharT, typename Traits>
        friend std::basic_istream<CharT, Traits>& operator>>(
            std::basic_istream<CharT, Traits>& stream,
            mersenne_twister_engine& engine
        ) {
            for (size_t i = 0; i < N; ++i) {
                stream >> engine.mt[i];
            }
            stream >> engine.index;
            return stream;
        }

    private:
        UIntT next() {
            if (index >= N)
                twist();
            UIntT y = mt[index];
            y ^= ((y >> U) & D);
            y ^= ((y << S) & B);
            y ^= ((y << T) & C);
            y ^= (y >> L);
            ++index;
            return y;
        }

        void twist() {
            constexpr UIntT lmask = (UIntT(1) << R) - 1;
            constexpr UIntT hmask = ~lmask;
            for (UIntT i = 0; i < N; ++i) {
                const UIntT x = (mt[i] & hmask) | (mt[(i + 1) % N] & lmask);
                const UIntT y = (x % 2 == 0) ? (x >> 1) : ((x >> 1) ^ A);
                mt[i] = mt[(i + M) % N] ^ y;
            }
            index = 0;
        }

    private:
        UIntT mt[N];
        UIntT index;
    };

    using mt19937 = mersenne_twister_engine<
        uint32_t, 32, 624, 397, 31,
        0X9908B0DF, 11,
        0XFFFFFFFF, 7,
        0X9D2C5680, 15,
        0XEFC60000, 18,
        1812433253>;

    using mt19937_64 = mersenne_twister_engine<
        uint64_t, 64, 312, 156, 31,
        0XB5026F5AA96619E9, 29,
        0x5555555555555555, 17,
        0X71D67FFFEDA60000, 37,
        0XFFF7EEE000000000, 43,
        6364136223846793005>;
}

// Demonstration

#include <iostream>
#include <random>
#include <sstream>

template <typename T, typename R>
static void perform_sequence_test() {
    static constexpr typename T::result_type seed = 19940720;
    T M1;
    R M2;
    M1.seed(seed);
    M2.seed(seed);
    for (int i = 0; i < 10; ++i) {
        std::cout << M1() << " =?= " << M2() << '\n';
    }
}

template <typename T, typename R>
static void perform_n10000_test() {
    T M1;
    R M2;
    M1.discard(9999);
    M2.discard(9999);
    std::cout << M1() << " =?= " << M2() << "\n";
}

template <typename T, typename R>
static void perform_max_test() {
    std::cout << T::max() << " =?= " << R::max() << "\n";
}

template <typename T, typename R>
static void perform_seed_test() {
    std::seed_seq S1{1, 2, 3, 4, 5};
    std::seed_seq S2{1, 2, 3, 4, 5};
    T M1(S1);
    R M2(S2);
    std::cout << M1() << " =?= " << M2() << '\n';
}

template <typename T, typename R>
static void perform_operators_test() {
    std::stringstream O1;
    std::stringstream O2;
    T M1(19940720);
    R M2(19940720);
    O1 << M1;
    O2 << M2;
    const std::string S1 = O1.str();
    const std::string S2 = O2.str();
    std::stringstream I1(S1);
    std::stringstream I2(S2);
    I1 >> M1;
    I2 >> M2;
    std::cout << M1() << " =?= " << M2() << '\n';
}

#define PERFORM_TESTS(name) \
    std::cout << "Comparing my::" #name " to std::" #name ".\n\n";  \
    perform_sequence_test<my::name, std::name>();                   \
    std::cout << "...\n";                                           \
    perform_n10000_test<my::name, std::name>();                     \
    std::cout << "...\n";                                           \
    perform_max_test<my::name, std::name>();                        \
    std::cout << "...\n";                                           \
    perform_seed_test<my::name, std::name>();                       \
    std::cout << "...\n";                                           \
    perform_operators_test<my::name, std::name>();                  \
    std::cout << '\n'


int main() {
    PERFORM_TESTS(mt19937);
    PERFORM_TESTS(mt19937_64);
    return EXIT_SUCCESS;
}
