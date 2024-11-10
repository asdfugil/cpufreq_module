#include <errno.h>
#include <pongo.h>

#include "cpufreq_private.h"
#include "utils.h"

uint32_t board_id;

__attribute__((visibility("default"))) char *module_name = "cpufreq";

__attribute__((visibility("default"))) void module_entry(void)
{
    switch (socnum) {
        case 0x8960:
            data.hw_config = &s5l8960x_config;
            break;
        case 0x7000:
            data.hw_config = &t7000_config;
            break;
        case 0x7001:
            data.hw_config = &t7001_config;
            break;
        case 0x8000:
        case 0x8001:
        case 0x8003:
            data.hw_config = &s800x_config;
            break;
        case 0x8010:
            data.hw_config = &t8010_config;
            break;
        case 0x8011:
            data.hw_config = &t8011_config;
            break;
        case 0x8012:
            data.hw_config = &t8012_config;
            break;
        case 0x8015:
            data.hw_config = &t8015_config;
            break;
        default:
            printf("cpufreq: Unsupported SoC\n");
            return;
    };

    board_id = *((uint32_t *)dt_prop(dt_find(gDeviceTree, "/chosen"), "board-id", NULL));

    write32(data.hw_config->voltage_ctl, 1);

    if (data.hw_config->hw_init)
        data.hw_config->hw_init(data.hw_config->cluster_base);

    uint64_t iboot_freq = get_frequency_for_state(data.hw_config->default_state);
    uint32_t max_config_state;
    for (max_config_state = data.hw_config->default_state + 1;
         max_config_state < data.hw_config->max_pstate; max_config_state++) {

        if (get_frequency_for_state(max_config_state) == iboot_freq)
            break;

        uint64_t addr = 0;

        switch (socnum) {
            case 0x8000:
            case 0x8001:
            case 0x8003:
                addr = data.hw_config->cluster_base + CLUSTER_PSINFO2_S8000(max_config_state);
                break;
            case 0x8010:
            case 0x8011:
            case 0x8012:
                addr = data.hw_config->cluster_base + CLUSTER_PSINFO2_T8010(max_config_state);
                ;
                break;
            case 0x8015:
                addr = data.hw_config->cluster_base + CLUSTER_PSINFO2_T8015(max_config_state);
                break;
        }

        if (addr && FIELD_GET(CLUSTER_PSINFO_MAX_LOAD, read64(addr)) != 15) {
            if (!data.max_nonboost_pstate)
                data.max_nonboost_pstate = max_config_state - 1;
        }
    }

    if (!data.max_nonboost_pstate)
        data.max_nonboost_pstate = max_config_state - 1;

    data.max_configured_pstate = max_config_state - 1;

    set64(data.hw_config->cluster_base + 0x200f8, BIT(40));

    printf("cpufreq: CPU state: %llu -> %u\n", get_state(), data.max_nonboost_pstate);
    set_state(data.max_nonboost_pstate);

    command_register("cpufreq", "CPU frequency scaling", cpufreq_cmd);
    return;
}

void s8000_hw_init(uint64_t cluster_base)
{
    clear64(cluster_base + CLUSTER_PSTATE_CMD, CLUSTER_PSTATE_APSC_DIS);
}

void t8015_hw_init(uint64_t cluster_base)
{
    s8000_hw_init(cluster_base);
    set64(cluster_base + CLUSTER_PSTATE_CMD, CLUSTER_PSTATE_PLL_EN);
}

__attribute__((visibility("default"))) struct pongo_exports exported_symbols[] = {NULL};
