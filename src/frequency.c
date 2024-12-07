#include <pongo.h>

#include "cpufreq_private.h"
#include "utils.h"

#define FREQ_INFO_OFF_S5L8960X 0x20068
#define FREQ_INFO_OFF_T8010    0x80000
#define FREQ_INFO_OFF_T8015    0x70000

#define CORE_TYPE_T8010 BIT(23)

uint64_t get_frequency_for_state_s5l8960x(uint64_t cluster_base, int state)
{
    uint64_t reg = read64(cluster_base + FREQ_INFO_OFF_S5L8960X + 8 * state);

    uint64_t P = FIELD_GET(P_S5L8960X, reg);
    uint64_t M = FIELD_GET(M_S5L8960X, reg);
    uint64_t S = FIELD_GET(S_S5L8960X, reg);

    return ((BASE_CLOCK * M) / P) / (S + 1);
}

uint64_t get_frequency_for_state_s8000(uint64_t cluster_base, int state)
{
    if (state < 8)
        return get_frequency_for_state_s5l8960x(cluster_base, state);
    else
        return get_frequency_for_state_s5l8960x(cluster_base + 0x300, state - 8);
}

static uint64_t t8015_freq_info_to_freq(uint64_t reg)
{
    uint64_t M = FIELD_GET(M_S8000, reg);
    uint64_t S = FIELD_GET(S_S8000, reg);

    return (M * BASE_CLOCK) / (S * 2 + 2);
}

static uint64_t t8010_freq_info_to_freq(uint64_t reg)
{
    uint64_t P = FIELD_GET(P_S8000, reg);
    uint64_t M = FIELD_GET(M_S8000, reg);
    uint64_t S = FIELD_GET(S_S8000, reg);

    return ((P / 5 + M) * BASE_CLOCK) / (2 * (S + 1));
}

uint64_t get_frequency_for_state_t8010(uint64_t cluster_base, int state)
{
    uint64_t reg = read64(cluster_base + FREQ_INFO_OFF_T8010 + 0x20 * state);
    return t8010_freq_info_to_freq(reg);
}

uint64_t get_frequency_for_state_t8015(uint64_t cluster_base, int state)
{
    uint64_t reg = read64(cluster_base + FREQ_INFO_OFF_T8015 + 0x20 * state);
    return t8015_freq_info_to_freq(reg);
}

enum core_type get_core_type_for_state_t8010(uint64_t cluster_base, int state)
{
    uint64_t reg = read64(cluster_base + FREQ_INFO_OFF_T8010 + 0x20 * state);
    return !!(reg & CORE_TYPE_T8010);
}