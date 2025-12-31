#include <pongo.h>

#include "cpufreq_private.h"
#include "utils.h"

/* Actually just overclock */

int apply_magic_s5l8960x(const struct cpufreq_hw_config *config)
{
    ((uint32_t *)dt_prop(dt_find(gDeviceTree, "/arm-io/pmgr"), "voltage-states", NULL))[12] =
        0xb7e8; // 1392 MHz
    ((uint32_t *)dt_prop(dt_find(gDeviceTree, "/arm-io/pmgr"), "voltage-states", NULL))[13] =
        0x4e2; // 1250 mV
    write64(config->cluster_base + CLUSTER_PSINFO1_S5L8960X(6),
            0x0000000007200000 | FIELD_PREP(CLUSTER_PSINFO1_VCORE_S8000, 0xd0) |
                FIELD_PREP(MUL_S5L8960X, 0x3a) | FIELD_PREP(DIV1_S5L8960X, 1) | FIELD_PREP(DIV2_S5L8960X, 0));

    return 6;
}

int apply_magic_s800x(const struct cpufreq_hw_config *config)
{
    ((uint32_t *)dt_prop(dt_find(gDeviceTree, "/arm-io/pmgr"), "voltage-states1", NULL))[12] =
        0x6893; // 2448 MHz
    ((uint32_t *)dt_prop(dt_find(gDeviceTree, "/arm-io/pmgr"), "voltage-states1", NULL))[13] =
        0x3e8; // 1000 mV
    write64(config->cluster_base + CLUSTER_PSINFO2_S8000(8),
            0x550000800d000000 | FIELD_PREP(CLUSTER_PSINFO2_MAX_LOAD, 15));
    write64(config->cluster_base + CLUSTER_PSINFO1_S5L8960X(8),
            0x0000000007200000 | FIELD_PREP(CLUSTER_PSINFO1_VCORE_S8000, 0xb0) |
                FIELD_PREP(MOD_S8000, 1) | FIELD_PREP(MUL_S8000, 0xcc) | FIELD_PREP(DIV_S8000, 0));

    return 8;
}

int apply_magic_t8010(const struct cpufreq_hw_config *config)
{
    uint8_t state = 11;
    // ipod touch 7
    if (board_id == 0x16) {
        state = 8;
    }

    // TODO: Tell the kernel about our configuration in DeviceTree
    // vcore = (psinfo1_vcore * 3125 - 500) / 1000 + 500 mV
    // vsram = (psinfo2_vsram * 3125 - 600) / 1000 + 600 mV

    write64(config->cluster_base + CLUSTER_PSINFO1_T8010(state),
            0x0000000404a20000 | FIELD_PREP(CLUSTER_PSINFO1_VCORE_S8000, 0xb8) |
                FIELD_PREP(MOD_S8000, 0) | FIELD_PREP(MUL_S8000, 0xc8) | FIELD_PREP(DIV_S8000, 0));
    write64(config->cluster_base + CLUSTER_PSINFO2_T8010(state),
            0x660020c68d000000 | FIELD_PREP(CLUSTER_PSINFO2_MAX_LOAD, 15));
    write64(config->cluster_base + CLUSTER_PSINFO3_T8010(state), 0x4800);

    return state;
}

int apply_magic_t8015(const struct cpufreq_hw_config *config)
{
#if 0
    printf("epsinfo1: 0x%llx\n", read64(config->cluster_base + CLUSTER_PSINFO1_T8015(7)));
    printf("epsinfo2: 0x%llx\n", read64(config->cluster_base + CLUSTER_PSINFO2_T8015(7)));
    printf("epsinfo3: 0x%llx\n", read64(config->cluster_base + CLUSTER_PSINFO3_T8015(7)));

    printf("ppsinfo1: 0x%llx\n", read64(config->pcluster_base + CLUSTER_PSINFO1_T8015(8)));
    printf("ppsinfo2: 0x%llx\n", read64(config->pcluster_base + CLUSTER_PSINFO2_T8015(8)));
    printf("ppsinfo3: 0x%llx\n", read64(config->pcluster_base + CLUSTER_PSINFO3_T8015(8)));
#endif

    // voltage-statesN to Hz = (65536000000000 / value) Hz
    ((uint32_t *)dt_prop(dt_find(gDeviceTree, "/arm-io/pmgr"), "voltage-states5", NULL))[12] =
        0x5d91; // 2736 MHz P-core
    ((uint32_t *)dt_prop(dt_find(gDeviceTree, "/arm-io/pmgr"), "voltage-states5", NULL))[13] =
        1062; // 1062 mV P-core
    ((uint32_t *)dt_prop(dt_find(gDeviceTree, "/arm-io/pmgr"), "voltage-states5-sram", NULL))[12] =
        2736000000; // 2736 MHz P-core
    ((uint32_t *)dt_prop(dt_find(gDeviceTree, "/arm-io/pmgr"), "voltage-states5-sram", NULL))[13] =
        1287; // 1287 mV P-core SRAM
    ((uint32_t *)dt_prop(dt_find(gDeviceTree, "/arm-io/pmgr"), "voltage-states1", NULL))[10] =
        0x7936; // 2112 MHz E-core
    ((uint32_t *)dt_prop(dt_find(gDeviceTree, "/arm-io/pmgr"), "voltage-states1", NULL))[11] =
        1062; // 1062 mV E-core
    ((uint32_t *)dt_prop(dt_find(gDeviceTree, "/arm-io/pmgr"), "voltage-states1-sram", NULL))[10] =
        2112000000; // 2112 MHz E-core
    ((uint32_t *)dt_prop(dt_find(gDeviceTree, "/arm-io/pmgr"), "voltage-states1-sram", NULL))[11] =
        1174; // 1174 mV E-core SRAM

    // vcore = (psinfo1_vcore * 3125 - 500) / 1000 + 375 mV
    // vsram = (psinfo2_vsram * 3125 - 500) / 1000 + 600 mV

    write64(config->cluster_base + CLUSTER_PSINFO1_T8015(7),
        0x0000000808200000 | FIELD_PREP(CLUSTER_PSINFO1_VCORE_S8000, 0xdc)
        | FIELD_PREP(MUL_S8000, 0xb0) | FIELD_PREP(DIV_S8000, 0));
    write64(config->cluster_base + CLUSTER_PSINFO2_T8015(7),
        0x450040008d000000 | FIELD_PREP(CLUSTER_PSINFO2_MAX_LOAD, 15)
        | FIELD_PREP(CLUSTER_PSINFO2_VSRAM_T8015, 0xb8));
    write64(config->cluster_base + CLUSTER_PSINFO3_T8015(7), 0x4600);

    write64(config->pcluster_base + CLUSTER_PSINFO1_T8015(8),
        0x0000000808200000 | FIELD_PREP(CLUSTER_PSINFO1_VCORE_S8000, 0xdc)
        | FIELD_PREP(MUL_S8000, 0xe4) | FIELD_PREP(DIV_S8000, 0));
    write64(config->pcluster_base + CLUSTER_PSINFO2_T8015(8),
        0x570040008d000000 | FIELD_PREP(CLUSTER_PSINFO2_MAX_LOAD, 15)
        | FIELD_PREP(CLUSTER_PSINFO2_VSRAM_T8015, 0xdc));
    write64(config->pcluster_base + CLUSTER_PSINFO3_T8015(8), 0x4900);

#if 0
    printf("epsinfo1: 0x%llx\n", read64(config->cluster_base + CLUSTER_PSINFO1_T8015(7)));
    printf("epsinfo2: 0x%llx\n", read64(config->cluster_base + CLUSTER_PSINFO2_T8015(7)));
    printf("epsinfo3: 0x%llx\n", read64(config->cluster_base + CLUSTER_PSINFO3_T8015(7)));

    printf("ppsinfo1: 0x%llx\n", read64(config->pcluster_base + CLUSTER_PSINFO1_T8015(8)));
    printf("ppsinfo2: 0x%llx\n", read64(config->pcluster_base + CLUSTER_PSINFO2_T8015(8)));
    printf("ppsinfo3: 0x%llx\n", read64(config->pcluster_base + CLUSTER_PSINFO3_T8015(8)));
#endif

    return 7;
}
