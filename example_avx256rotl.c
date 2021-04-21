/* SPDX-License-Identifier:	MIT */

#include <stdio.h>
#include <immintrin.h>

#include "benchmark_toolkit.h"

/* for vectorization efficency comparasion */
static BENCHMARK_HELPER_INLINE __m256i _mm256_rotl_epi32(__m256i v, uint32_t r)
{
    return _mm256_or_si256(_mm256_slli_epi32(v, r), _mm256_srli_epi32(v, (32 - r)));
}

/* the most common form of implementation for 'rotl32' */
static BENCHMARK_HELPER_INLINE uint32_t benchmark_example_rotl32(uint32_t v, uint32_t r)
{
    return (v << r) | (v >> (32 - r));
}

int main(BENCHMARK_HELPER_UNUSED int argc, BENCHMARK_HELPER_UNUSED char *argv[] )
{
    BENCHMARK_HELPER_ALIGN const uint32_t benchmark_data_sample[4] = {0xDEADBEEF, 0xCAFEBABE, 0xFEEDF00D, 0xFACEC0DE};
    BENCHMARK_HELPER_ALIGN size_t benchmark_input_arrsz;
    BENCHMARK_HELPER_ALIGN uint32_t becnmark_input_rotate;
    BENCHMARK_HELPER_ALIGN uint32_t *benchmark_testarr_srcset;
    BENCHMARK_HELPER_ALIGN uint32_t *benchmark_testarr_normal;
    BENCHMARK_HELPER_ALIGN uint32_t *benchmark_testarr_vector;
    BENCHMARK_HELPER_ALIGN uint64_t benchmark_result_normal;
    BENCHMARK_HELPER_ALIGN uint64_t benchmark_result_vector;
    BENCHMARK_HELPER_ALIGN size_t benchmark_variable_idx;

    if (!BENCHMARK_COUNTER_TRUSTABLE()) {
        return ENOSYS;
    }

    fprintf(stdout, "Benchmark size (mul of 16) : ");
    scanf("%zu", &benchmark_input_arrsz);

    fprintf(stdout, "Rotation count (<= 32) : ");
    scanf("%u", &becnmark_input_rotate);

    benchmark_testarr_srcset = aligned_alloc(BENCHMARK_DEFINE_PAGESIZE,
        sizeof(*benchmark_testarr_srcset) * benchmark_input_arrsz);
    benchmark_testarr_normal = aligned_alloc(BENCHMARK_DEFINE_PAGESIZE,
        sizeof(*benchmark_testarr_normal) * benchmark_input_arrsz);
    benchmark_testarr_vector = aligned_alloc(BENCHMARK_DEFINE_PAGESIZE,
        sizeof(*benchmark_testarr_vector) * benchmark_input_arrsz);
    if (!benchmark_testarr_srcset || !benchmark_testarr_normal || !benchmark_testarr_vector) {
        if (benchmark_testarr_srcset)
            free(benchmark_testarr_srcset);
        if (benchmark_testarr_normal)
            free(benchmark_testarr_normal);
        if (benchmark_testarr_vector)
            free(benchmark_testarr_vector);

        fprintf(stderr, "Failed to allocate\n");
        return ENOMEM;
    }

    for (benchmark_variable_idx = 0; benchmark_variable_idx < benchmark_input_arrsz; benchmark_variable_idx += 4) {
        benchmark_testarr_srcset[benchmark_variable_idx + 0] = benchmark_data_sample[0];
        benchmark_testarr_srcset[benchmark_variable_idx + 1] = benchmark_data_sample[1];
        benchmark_testarr_srcset[benchmark_variable_idx + 2] = benchmark_data_sample[2];
        benchmark_testarr_srcset[benchmark_variable_idx + 3] = benchmark_data_sample[3];
    }

    for (benchmark_variable_idx = 0; benchmark_variable_idx < benchmark_input_arrsz; benchmark_variable_idx += BENCHMARK_DEFINE_PAGESIZE) {
        BENCHMARK_BENCHTOOL_PREFAULT(&benchmark_testarr_srcset[benchmark_variable_idx]);
        BENCHMARK_BENCHTOOL_PREFAULT(&benchmark_testarr_normal[benchmark_variable_idx]);
        BENCHMARK_BENCHTOOL_PREFAULT(&benchmark_testarr_vector[benchmark_variable_idx]);
    }

    for (benchmark_variable_idx = 0; benchmark_variable_idx < benchmark_input_arrsz; benchmark_variable_idx += BENCHMARK_DEFINE_CACHESIZE) {
        BENCHMARK_BENCHTOOL_CLFLUSH(&benchmark_testarr_srcset[benchmark_variable_idx]);
        BENCHMARK_BENCHTOOL_CLFLUSH(&benchmark_testarr_normal[benchmark_variable_idx]);
        BENCHMARK_BENCHTOOL_CLFLUSH(&benchmark_testarr_vector[benchmark_variable_idx]);
    }

    BENCHMARK_TEMPLATE_PREPARE();

    BENCHMARK_TEMPLATE_TESTCASE(benchmark_result_normal, {
        for (benchmark_variable_idx = 0; benchmark_variable_idx < benchmark_input_arrsz; ++benchmark_variable_idx) {
            benchmark_testarr_normal[benchmark_variable_idx] = benchmark_example_rotl32(benchmark_testarr_srcset[benchmark_variable_idx], becnmark_input_rotate);
        }
    });

    BENCHMARK_TEMPLATE_TESTCASE(benchmark_result_vector, {
        for (benchmark_variable_idx = 0; benchmark_variable_idx < benchmark_input_arrsz; benchmark_variable_idx += 0x10) {
            _mm_prefetch( &benchmark_testarr_srcset[benchmark_variable_idx + 0x10], _MM_HINT_NTA );
            /* Calculate two times for aligning to the cache line size */
            _mm256_stream_si256((__m256i *)&benchmark_testarr_vector[benchmark_variable_idx+0x0],
                    _mm256_rotl_epi32(_mm256_stream_load_si256((__m256i *)&benchmark_testarr_srcset[benchmark_variable_idx+0x0]), becnmark_input_rotate));
            _mm256_stream_si256((__m256i *)&benchmark_testarr_vector[benchmark_variable_idx+0x8],
                    _mm256_rotl_epi32(_mm256_stream_load_si256((__m256i *)&benchmark_testarr_srcset[benchmark_variable_idx+0x8]), becnmark_input_rotate));
        }
    });

    fprintf(stdout, "[NORMAL]: CYCLE=%020"PRIu64"\n", benchmark_result_normal);
    fprintf(stdout, "[VECTOR]: CYCLE=%020"PRIu64"\n", benchmark_result_vector);

    free(benchmark_testarr_srcset);
    free(benchmark_testarr_normal);
    free(benchmark_testarr_vector);

    return 0;
}
