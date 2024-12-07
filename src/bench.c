#include <pongo.h>

#include "cpufreq_private.h"
#include "utils.h"

#define BENCH_LOOPS 10000000

__attribute__((optnone)) __attribute__((naked)) uint64_t bench_internal(uint64_t loops)
{
    __asm__ volatile("mrs x1, cntpct_el0\n"
                     "1:\n"
                     "sub x0, x0, #1\n"
                     "cbnz x0, 1b\n"
                     "mrs x2, cntpct_el0\n"
                     "sub x0, x2, x1\n"
                     "ret\n");
}

__attribute__((optnone)) uint64_t bench(void)
{
    disable_interrupts();

    uint64_t ticks = bench_internal(BENCH_LOOPS);

    enable_interrupts();

    double elapsed = ticks / (double)BASE_CLOCK;

    if (elapsed == 0)
        return 0;

    uint64_t hz = (BENCH_LOOPS / (double)elapsed);

    return hz;
}