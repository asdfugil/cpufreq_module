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

    uint64_t div1 = FIELD_GET(DIV1_S5L8960X, reg);
    uint64_t mul = FIELD_GET(MUL_S5L8960X, reg);
    uint64_t div2 = FIELD_GET(DIV2_S5L8960X, reg);

    return ((BASE_CLOCK * mul) / div1) / (div2 + 1);
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
    uint64_t mul = FIELD_GET(MUL_S8000, reg);
    uint64_t div = FIELD_GET(DIV_S8000, reg);

    /*
     * could write as a multiplier of 12 mhz but it makes
     * more sense that everything follows the 24MHz clock ?
     */
    return (mul * BASE_CLOCK) / (2 * (div + 1));
}

static uint64_t t8010_freq_info_to_freq(uint64_t reg)
{
    uint64_t mod = FIELD_GET(MOD_S8000, reg);
    uint64_t mul = FIELD_GET(MUL_S8000, reg);
    uint64_t div = FIELD_GET(DIV_S8000, reg);

    return ((mod / 5 + mul) * BASE_CLOCK) / (2 * (div + 1));
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