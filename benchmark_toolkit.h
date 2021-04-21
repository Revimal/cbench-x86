/* SPDX-License-Identifier: MIT */

#ifndef BENCHMARK_TOOLKIT_H
#define BENCHMARK_TOOLKIT_H

#include <inttypes.h>
#include <errno.h>

/* TODO: CHANGE THE FOLLOWING VALUES TO SUIT YOUR ENVIRONMENT */
#define BENCHMARK_DEFINE_CACHESIZE 64
#define BENCHMARK_DEFINE_PAGESIZE 4096

#define BENCHMARK_HELPER_INLINE \
    __attribute__((always_inline)) inline

#define BENCHMARK_HELPER_UNUSED \
    __attribute__((unused))

#define BENCHMARK_HELPER_ALIGN \
    __attribute__((aligned(BENCHMARK_DEFINE_CACHESIZE)))

/* Referenced from:
 * 'Intel® 64 and IA-32 Architectures Software Developer’s Manual'
 * 'Combined Volumes:1, 2A, 2B, 2C, 2D, 3A, 3B, 3C, 3D and 4'
 * 'CPUID — CPU Identification'
 */
static BENCHMARK_HELPER_INLINE uint32_t benchmark_internal_chktsc(void)
{
    register uint32_t eax __asm__( "eax" );
    register uint32_t ebx __asm__( "ebx" );
    register uint32_t ecx __asm__( "ecx" );
    register uint32_t edx __asm__( "edx" );

    eax = 0x80000007;
    ecx = 0;

    __asm__ __volatile__ ("cpuid"
            : "+a" (eax), "=b" (ebx), "=d" (edx), "+c" (ecx)
            :: "memory");

    /* Get the resolved result for extended features (as bool) */
    return ((edx >> 8) & 0x1);
}

/* Referenced from:
 * 'Intel® 64 and IA-32 Architectures Software Developer’s Manual'
 * 'Combined Volumes:1, 2A, 2B, 2C, 2D, 3A, 3B, 3C, 3D and 4'
 * 'CLFLUSH — Flush Cache Line'
 */
static BENCHMARK_HELPER_INLINE void benchmark_internal_clflush(const void *addr)
{
    __asm__ __volatile__ (
        "clflush 0(%0)"
        :: "r" (addr)
        : "memory");

    return;
}

/* Accessing to the TSC and fill its value into a 64-bit variable */
static BENCHMARK_HELPER_INLINE uint64_t benchmark_internal_rdtsc(void)
{
    register uint32_t l32 __asm__("eax");
    register uint32_t h32 __asm__("edx");
    __asm__ __volatile__ (
            "rdtsc"
            : "=a" (l32), "=d" (h32)
            :: "memory");
    return ((uint64_t)h32 << 32) | ((uint64_t)l32);
}

#define BENCHMARK_BENCHTOOL_PREFAULT(addr) \
    *(volatile int *)addr = *(volatile int *)addr

#define BENCHMARK_BENCHTOOL_CLFLUSH(addr) \
    benchmark_internal_clflush((addr))

#define BENCHMARK_BENCHTOOL_VECOUT() \
    __asm__ __volatile__ ("vzeroupper")

#define BENCHMARK_BENCHTOOL_MFENCH() \
    __asm__ __volatile__ ("mfence")

#define BENCHMARK_BENCHTOOL_MEMORY() \
    __asm__ __volatile__ ("lock; addl $0,-4(%%rsp)" ::: "memory", "cc")

#define BENCHMARK_BENCHTOOL_COMPILER() \
    __asm__ __volatile__ ("": : :"memory")

#define BENCHMARK_MARKER_DBGPOINT() \
    __asm__ __volatile__ ("nop")

#define BENCHMARK_COUNTER_TRUSTABLE() \
    benchmark_internal_chktsc()

#define BENCHMARK_COUNTER_RELAX() \
    __asm__ __volatile__ ("pause")

#define BENCHMARK_COUNTER_GET() \
    benchmark_internal_rdtsc()

#define BENCHMARK_TEMPLATE_PREPARE() \
    do \
    { \
        BENCHMARK_BENCHTOOL_VECOUT(); \
        BENCHMARK_BENCHTOOL_MFENCH(); \
        BENCHMARK_BENCHTOOL_MEMORY(); \
    } while (0)

#define BENCHMARK_TEMPLATE_TESTCASE(result, body) \
    do \
    { \
        BENCHMARK_HELPER_ALIGN uint64_t timestamp_0; \
        BENCHMARK_HELPER_ALIGN uint64_t timestamp_1; \
        timestamp_0 = BENCHMARK_COUNTER_GET(); \
        BENCHMARK_MARKER_DBGPOINT(); \
        BENCHMARK_BENCHTOOL_COMPILER(); \
        {body} \
        BENCHMARK_BENCHTOOL_VECOUT(); \
        BENCHMARK_BENCHTOOL_MFENCH(); \
        BENCHMARK_BENCHTOOL_MEMORY(); \
        BENCHMARK_MARKER_DBGPOINT(); \
        timestamp_1 = BENCHMARK_COUNTER_GET(); \
        (result) = (timestamp_1 - timestamp_0); \
    } while (0)

#endif /* BENCHMARK_TOOLKIT_H */
