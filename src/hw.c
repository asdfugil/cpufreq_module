#include <pongo.h>

#include "cpufreq_private.h"
#include "utils.h"

const struct cpufreq_hw_config s5l8960x_config = {
    .max_pstate = 7,
    .default_state = 2,
    .apply_magic = apply_magic_s5l8960x,
    .get_vcore = get_vcore_s5l8960x,
    .cluster_base = 0x202200000,
    .voltage_ctl = 0x20e020c00,
    .get_frequency_for_state = get_frequency_for_state_s5l8960x,
    .set_state = set_state_s5l8960x,
    .get_state = get_state_s5l8960x,
    .get_target_state = get_target_state_s5l8960x,
};

const struct cpufreq_hw_config t7000_config = {
    .max_pstate = 7,
    .default_state = 2,
    .cluster_base = 0x202200000,
    .voltage_ctl = 0x20e023000,
    .get_frequency_for_state = get_frequency_for_state_s5l8960x,
    .set_state = set_state_s5l8960x,
    .get_state = get_state_s5l8960x,
    .get_target_state = get_target_state_s5l8960x,
};

const struct cpufreq_hw_config t7001_config = {
    .max_pstate = 7,
    .default_state = 2,
    .cluster_base = 0x202200000,
    .voltage_ctl = 0x20e023000,
    .get_frequency_for_state = get_frequency_for_state_s5l8960x,
    .set_state = set_state_s5l8960x,
    .get_state = get_state_s5l8960x,
    .get_target_state = get_target_state_s5l8960x,
};

const struct cpufreq_hw_config s800x_config = {
    .max_pstate = 15,
    .default_state = 2,
    .apply_magic = apply_magic_s800x,
    .get_vcore = get_vcore_s800x,
    .cluster_base = 0x202200000,
    .voltage_ctl = 0x20e0a0000,
    .hw_init = s8000_hw_init,
    .get_frequency_for_state = get_frequency_for_state_s8000,
    .set_state = set_state_s8000,
    .get_state = get_state_s8000,
    .get_target_state = get_target_state_s8000,
};

const struct cpufreq_hw_config t8010_config = {
    .max_pstate = 15,
    .default_state = 2,
    //.get_vcore = get_vcore_t8010,
    .apply_magic = apply_magic_t8010,
    .cluster_base = 0x202f00000,
    .voltage_ctl = 0x20e0a0000,
    .hw_init = s8000_hw_init,
    .get_core_type_for_state = get_core_type_for_state_t8010,
    .get_frequency_for_state = get_frequency_for_state_t8010,
    .set_state = set_state_s8000,
    .get_state = get_state_s8000,
    .get_target_state = get_target_state_s8000,
};

const struct cpufreq_hw_config t8011_config = {
    .max_pstate = 15,
    .default_state = 2,
    //.get_vcore = get_vcore_t8010,
    .cluster_base = 0x202f00000,
    .voltage_ctl = 0x20e0a0000,
    .hw_init = s8000_hw_init,
    .get_core_type_for_state = get_core_type_for_state_t8010,
    .get_frequency_for_state = get_frequency_for_state_t8010,
    .set_state = set_state_s8000,
    .get_state = get_state_s8000,
    .get_target_state = get_target_state_s8000,
};

const struct cpufreq_hw_config t8012_config = {
    .max_pstate = 15,
    .default_state = 2,
    .apply_magic = apply_magic_t8010,
    //.get_vcore = get_vcore_t8010,
    .cluster_base = 0x202f00000,
    .voltage_ctl = 0x20e0a0000,
    .hw_init = s8000_hw_init,
    .get_core_type_for_state = get_core_type_for_state_t8010,
    .get_frequency_for_state = get_frequency_for_state_t8010,
    .set_state = set_state_s8000,
    .get_state = get_state_s8000,
    .get_target_state = get_target_state_s8000,
};

const struct cpufreq_hw_config t8015_config = {
    .max_pstate = 15,
    .default_state = 2,
    .apply_magic = apply_magic_t8015,
    .cluster_base = 0x208e00000,
    .pcluster_base = 0x208e80000,
    .voltage_ctl = 0x2320a0000,
    .hw_init = t8015_hw_init,
    .get_frequency_for_state = get_frequency_for_state_t8015,
    .set_state = set_state_s8000,
    .get_state = get_state_s8000,
    .get_target_state = get_target_state_s8000,
};
