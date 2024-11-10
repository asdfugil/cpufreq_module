#include <pongo.h>

#include "cpufreq_private.h"
#include "utils.h"

/* Actually just overclock */

int apply_magic_s800x(const struct cpufreq_hw_config *config)
{
    ((uint32_t *)dt_prop(dt_find(gDeviceTree, "/arm-io/pmgr"), "voltage-states1", NULL))[12] =
        0x6893; // 2448 MHz
    ((uint32_t *)dt_prop(dt_find(gDeviceTree, "/arm-io/pmgr"), "voltage-states1", NULL))[13] =
        0x3e8; // 1000 mV
    write64(config->cluster_base + CLUSTER_PSINFO2_S8000(8),
            0x550000800d000000 | FIELD_PREP(CLUSTER_PSINFO_MAX_LOAD, 15));
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

    write64(config->cluster_base + CLUSTER_PSINFO1_T8010(state),
            0x0000000404a20000 | FIELD_PREP(CLUSTER_PSINFO1_VCORE_S8000, 0xb8) |
                FIELD_PREP(MOD_S8000, 0) | FIELD_PREP(MUL_S8000, 0xc8) | FIELD_PREP(DIV_S8000, 0));
    write64(config->cluster_base + CLUSTER_PSINFO2_T8010(state),
            0x660020c68d000000 | FIELD_PREP(CLUSTER_PSINFO_MAX_LOAD, 15));
    write64(config->cluster_base + CLUSTER_PSINFO3_T8010(state), 0x4800);

    return state;
}

int apply_magic_t8015(const struct cpufreq_hw_config *config)
{
    ((uint32_t *)dt_prop(dt_find(gDeviceTree, "/arm-io/pmgr"), "voltage-states5", NULL))[12] =
        0x5d91; // 2736 MHz P-core
    ((uint32_t *)dt_prop(dt_find(gDeviceTree, "/arm-io/pmgr"), "voltage-states5", NULL))[13] =
        1062; // 1062 mV P-core
    ((uint32_t *)dt_prop(dt_find(gDeviceTree, "/arm-io/pmgr"), "voltage-states5-sram", NULL))[12] =
        2736000000; // 2736 MHz P-core
    ((uint32_t *)dt_prop(dt_find(gDeviceTree, "/arm-io/pmgr"), "voltage-states5-sram", NULL))[13] =
        1062; // 1062 mV P-core SRAM
    ((uint32_t *)dt_prop(dt_find(gDeviceTree, "/arm-io/pmgr"), "voltage-states1", NULL))[10] =
        0x7936; // 2112 MHz E-core
    ((uint32_t *)dt_prop(dt_find(gDeviceTree, "/arm-io/pmgr"), "voltage-states1", NULL))[11] =
        1062; // 1062 mV E-core
    ((uint32_t *)dt_prop(dt_find(gDeviceTree, "/arm-io/pmgr"), "voltage-states1-sram", NULL))[10] =
        2112000000; // 2112 MHz E-core
    ((uint32_t *)dt_prop(dt_find(gDeviceTree, "/arm-io/pmgr"), "voltage-states1-sram", NULL))[11] =
        949; // 949 mV E-core SRAM

    /*
    write64(0x208e70000 + 0 + 7 * 0x20, 0xcc00000808200a00);
    write64(0x208e70000 + 8 + 7 * 0x20, 0x45004f898d000000);
    */
    write64(0x208e70000 + 0 + 7 * 0x20, 0xdc00000808200b00);
    write64(0x208e70000 + 8 + 7 * 0x20, 0x45004fb88d000000);

    write64(0x208e70000 + 0x10 + 7 * 0x20, 0x4600);

    write64(0x208ef0000 + 0 + 8 * 0x20, 0xdc00000808200e40);
    write64(0x208ef0000 + 8 + 8 * 0x20, 0x57004fdc8d000000);
    write64(0x208ef0000 + 0x10 + 8 * 0x20, 0x4900);

    return 7;
}
