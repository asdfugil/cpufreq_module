#include <pongo.h>

#include "cpufreq_private.h"
#include "utils.h"

int set_state_s5l8960x(uint64_t cluster_base, int state)
{

    uint64_t reg = 0;
    reg |= FIELD_PREP(CLUSTER_PSTATE_CMD_PS3, state);
    reg |= CLUSTER_PSTATE_CMD_SET;

    mask64(cluster_base + CLUSTER_PSTATE_CMD, CLUSTER_PSTATE_CMD_PS3, reg);
    if (poll32(cluster_base + CLUSTER_PSTATE_STATUS,
               CLUSTER_PSTATE_STATUS_ACTUAL_PS_S5L8960X | CLUSTER_PSTATE_STATUS_TARGET_PS_S5L8960X,
               state | state << 3, 1000)) {
        printf("cpufreq: P-state switching timed out\n");

        return -1;
    }
    return 0;
}

int set_state_s8000(uint64_t cluster_base, int state)
{
    uint64_t reg = 0;
    reg |= FIELD_PREP(CLUSTER_PSTATE_CMD_PS1, state);
    reg |= FIELD_PREP(CLUSTER_PSTATE_CMD_PS2, state);
    reg |= CLUSTER_PSTATE_CMD_SET;

    if (poll64(cluster_base + CLUSTER_PSTATE_CMD, CLUSTER_PSTATE_CMD_BUSY, 0,
               CLUSTER_PSTATE_TRANSITION_TIMEOUT) < 0) {
        printf("cpufreq: Failed to set CPU state\n");
        return -1;
    }

    write64(cluster_base + CLUSTER_PSTATE_CMD, reg);
    // mask64(cluster_base + CLUSTER_PSTATE_CMD, CLUSTER_PSTATE_CMD_PS1 | CLUSTER_PSTATE_CMD_PS2,
    // reg);
    if (poll32(cluster_base + CLUSTER_PSTATE_STATUS,
               CLUSTER_PSTATE_STATUS_TARGET_PDIV_S8000 | CLUSTER_PSTATE_STATUS_ACTUAL_PS_S8000,
               state | state << 4, 1000)) {
        printf("cpufreq: P-state switching timed out. Current state: %llu\n", get_state());

        if (state >= data.max_nonboost_pstate)
            printf("cpufreq: Boost states are locked. Unlock them using `cpufreq unlock`.\n(Device "
                   "may become unstable)\n");

        return -1;
    }
    return 0;
}

uint32_t get_state_s5l8960x(uint64_t cluster_base)
{
    uint32_t reg = read32(cluster_base + CLUSTER_PSTATE_STATUS);
    return FIELD_GET(CLUSTER_PSTATE_STATUS_TARGET_PS_S5L8960X, reg);
}

uint32_t get_target_state_s5l8960x(uint64_t cluster_base)
{
    uint32_t reg = read32(cluster_base + CLUSTER_PSTATE_STATUS);
    return FIELD_GET(CLUSTER_PSTATE_STATUS_TARGET_PS_S5L8960X, reg);
}

uint32_t get_state_s8000(uint64_t cluster_base)
{
    uint32_t reg = read32(cluster_base + CLUSTER_PSTATE_STATUS);
    return FIELD_GET(CLUSTER_PSTATE_STATUS_ACTUAL_PS_S8000, reg);
}

uint32_t get_target_state_s8000(uint64_t cluster_base)
{
    uint32_t reg = read32(cluster_base + CLUSTER_PSTATE_STATUS);
    return FIELD_GET(CLUSTER_PSTATE_STATUS_TARGET_PDIV_S8000, reg);
}
