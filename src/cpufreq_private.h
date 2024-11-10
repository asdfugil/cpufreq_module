#ifndef _CPUFREQ_PRIVATE_H_
#define _CPUFREQ_PRIVATE_H_

#include <stdint.h>

#define CLUSTER_PSTATE_CMD      0x20020
#define CLUSTER_PSTATE_CMD_BUSY BIT(31)
#define CLUSTER_PSTATE_CMD_SET  BIT(25)
#define CLUSTER_PSTATE_CMD_PS3  GENMASK(24, 22)
#define CLUSTER_PSTATE_CMD_PS2  GENMASK(15, 12)
#define CLUSTER_PSTATE_CMD_PS1  GENMASK(4, 0)
#define CLUSTER_PSTATE_APSC_DIS BIT(22)
#define CLUSTER_PSTATE_PLL_EN   BIT(42)

#define CLUSTER_PSTATE_STATUS                    0x20050
#define CLUSTER_PSTATE_STATUS_TARGET_PS_S5L8960X GENMASK(2, 0)
#define CLUSTER_PSTATE_STATUS_ACTUAL_PS_S5L8960X GENMASK(5, 3)
#define CLUSTER_PSTATE_STATUS_TARGET_PS_S8000    GENMASK(3, 0)
#define CLUSTER_PSTATE_STATUS_ACTUAL_PS_S8000    GENMASK(7, 4)

enum core_type { CORE_TYPE_E, CORE_TYPE_P, CORE_TYPE_UNKNOWN };

static char *core_type_array[] = {"E-Core", "P-Core", "Unknown"};

struct cpufreq_command {
    char *name;
    char *desc;
    void (*cb)(const char *cmd, char *args);
};

struct cpufreq_data {
    uint32_t max_configured_pstate;
    const struct cpufreq_hw_config *hw_config;
} data;

struct cpufreq_hw_config {
    uint32_t max_pstate;
    uint32_t iboot_state;
    uint32_t pongo_state;
    uint64_t cluster_base;
    uint64_t voltage_ctl;
    void (*hw_init)(uint64_t cluster_base);
    enum core_type (*get_core_type_for_state)(uint64_t cluster_base, int state);
    uint64_t (*get_frequency_for_state)(uint64_t cluster_base, int state);
    int (*set_state)(uint64_t cluster_base, int state);
    uint32_t (*get_state)(uint64_t cluster_base);
    uint32_t (*get_target_state)(uint64_t cluster_base);
};

extern const struct cpufreq_hw_config s5l8960x_config, t7000_config, t7001_config, s800x_config,
    t8010_config, t8011_config, t8012_config, t8015_config;

void cpufreq_cmd(const char *cmd, char *args);

/* Generic functions */
uint64_t get_frequency_for_state(int state);
int set_state(int state);
uint64_t get_state(void);
uint64_t get_frequency_for_state(int state);
uint64_t get_core_type_for_state(int state);

/* SoC-specific frequency functions */
uint64_t get_frequency_for_state_s5l8960x(uint64_t cluster_base, int state);
uint64_t get_frequency_for_state_s8000(uint64_t cluster_base, int state);
uint64_t get_frequency_for_state_t8010(uint64_t cluster_base, int state);
uint64_t get_frequency_for_state_t8015(uint64_t cluster_base, int state);

/* Apple Fusion functions */
enum core_type get_core_type_for_state_t8010(uint64_t cluster_base, int state);

/* SoC-specific State functions */
int set_state_s5l8960x(uint64_t cluster_base, int state);
int set_state_s8000(uint64_t cluster_base, int state);
uint32_t get_state_s5l8960x(uint64_t cluster_base);
uint32_t get_target_state_s5l8960x(uint64_t cluster_base);
uint32_t get_state_s8000(uint64_t cluster_base);
uint32_t get_target_state_s8000(uint64_t cluster_base);

/* SoC-specific init functions */
void s8000_hw_init(uint64_t cluster_base);
void t8015_hw_init(uint64_t cluster_base);
#endif