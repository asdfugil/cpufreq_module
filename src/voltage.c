#include <pongo.h>

#include "cpufreq_private.h"
#include "utils.h"

/* Core voltage */

uint32_t get_vcore_s5l8960x(uint64_t cluster_base, uint32_t state)
{
    uint64_t psinfo1 = read64(cluster_base + CLUSTER_PSINFO1_S5L8960X(state));
    uint32_t v = FIELD_GET(CLUSTER_PSINFO1_VCORE_S8000, psinfo1);
    return 600 + ((v * 3125) / 1000);
}

uint32_t get_vcore_s800x(uint64_t cluster_base, uint32_t state)
{
    uint64_t psinfo1 = read64(cluster_base + CLUSTER_PSINFO1_S5L8960X(state));
    uint32_t v = FIELD_GET(CLUSTER_PSINFO1_VCORE_S8000, psinfo1);
    return 450 + ((v * 3125) / 1000);
}

// WIP: Does not work properly for state 0 and 1
uint32_t get_vcore_t8010(uint64_t cluster_base, uint32_t state)
{
    uint64_t psinfo1 = read64(cluster_base + CLUSTER_PSINFO1_T8010(state));
    uint32_t v = FIELD_GET(CLUSTER_PSINFO1_VCORE_S8000, psinfo1);
    return 500 + ((v * 3125 - 500) / 1000);
}

uint32_t get_vcore_t8015(uint64_t cluster_base, uint32_t state)
{
    uint64_t psinfo1 = read64(cluster_base + CLUSTER_PSINFO1_T8010(state));
    uint32_t v = FIELD_GET(CLUSTER_PSINFO1_VCORE_S8000, psinfo1);
    return 375 + ((v * 3125 - 500) / 1000);
}
