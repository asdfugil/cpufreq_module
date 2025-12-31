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
#define CLUSTER_PSTATE_STATUS_TARGET_PDIV_S8000  GENMASK(3, 0)
#define CLUSTER_PSTATE_STATUS_ACTUAL_PS_S8000    GENMASK(7, 4)

#define CLUSTER_PSTATE_TRANSITION_TIMEOUT 400

#define DIV1_S5L8960X GENMASK(17, 13)
#define MUL_S5L8960X  GENMASK(12, 4)
#define DIV2_S5L8960X GENMASK(3, 0)

#define MOD_S8000 GENMASK(16, 14)
#define MUL_S8000 GENMASK(13, 4)
#define DIV_S8000 GENMASK(3, 0)

#define BASE_CLOCK 24000000

// PSINFO: holds the frequency, voltage and boost information for each P-State
// state 0 - 7, PSINFO1
#define CLUSTER_PSINFO1_OFF_S5L8960X 0x20068
// state 8 - 15, PSINFO1
#define CLUSTER_PSINFO1_EXT_OFF_S8000 0x20368
// state 0 - 15, PSINFO2
#define CLUSTER_PSINFO2_OFF_S8000 0x20868

// NOTE: state 8 - 15 is only available on S800X
#define CLUSTER_PSINFO1_S5L8960X(pstate)                                                           \
    ((pstate >= 8) ? (CLUSTER_PSINFO1_EXT_OFF_S8000 + (pstate - 8) * 8)                            \
                   : (CLUSTER_PSINFO1_OFF_S5L8960X + (pstate * 8)))
#define CLUSTER_PSINFO2_S8000(pstate) (CLUSTER_PSINFO2_OFF_S8000 + (pstate * 8))

// Starting from T8010 it's much saner with all the PSINFO's in strides
#define CLUSTER_PSINFO_STRIDE_T8010 0x20

#define CLUSTER_PSINFO_OFF_T8010 0x80000
#define CLUSTER_PSINFO_OFF_T8015 0x70000

#define CLUSTER_PSINFO1_T8010(pstate)                                                              \
    (CLUSTER_PSINFO_OFF_T8010 + (pstate * CLUSTER_PSINFO_STRIDE_T8010))
#define CLUSTER_PSINFO2_T8010(pstate)                                                              \
    (CLUSTER_PSINFO_OFF_T8010 + 8 + (pstate * CLUSTER_PSINFO_STRIDE_T8010))
#define CLUSTER_PSINFO3_T8010(pstate)                                                              \
    (CLUSTER_PSINFO_OFF_T8010 + 0x10 + (pstate * CLUSTER_PSINFO_STRIDE_T8010))

#define CLUSTER_PSINFO1_T8015(pstate)                                                              \
    (CLUSTER_PSINFO_OFF_T8015 + (pstate * CLUSTER_PSINFO_STRIDE_T8010))
#define CLUSTER_PSINFO2_T8015(pstate)                                                              \
    (CLUSTER_PSINFO_OFF_T8015 + 8 + (pstate * CLUSTER_PSINFO_STRIDE_T8010))
#define CLUSTER_PSINFO3_T8015(pstate)                                                              \
    (CLUSTER_PSINFO_OFF_T8015 + 0x10 + (pstate * CLUSTER_PSINFO_STRIDE_T8010))

#define CLUSTER_PSINFO2_MAX_LOAD GENMASK(43, 40)

#define CLUSTER_PSINFO1_VCORE_S8000 GENMASK(63, 56)
#define CLUSTER_PSINFO2_VSRAM_T8015 GENMASK(39, 32)

enum core_type { CORE_TYPE_E, CORE_TYPE_P, CORE_TYPE_UNKNOWN };

static char *core_type_array[] = {"E-Core", "P-Core", "Unknown"};

struct cpufreq_command {
    char *name;
    char *desc;
    void (*cb)(const char *cmd, char *args);
};

struct cpufreq_data {
    uint32_t max_configured_pstate;
    uint32_t max_nonboost_pstate;
    const struct cpufreq_hw_config *hw_config;
} data;

struct cpufreq_hw_config {
    uint32_t max_pstate;
    uint32_t default_state;
    uint64_t cluster_base;
    uint64_t pcluster_base;
    uint64_t voltage_ctl;
    int (*apply_magic)(const struct cpufreq_hw_config *config);
    uint32_t (*get_vcore)(uint64_t cluster_base, uint32_t state);
    void (*hw_init)(uint64_t cluster_base);
    enum core_type (*get_core_type_for_state)(uint64_t cluster_base, int state);
    uint64_t (*get_frequency_for_state)(uint64_t cluster_base, int state);
    int (*set_state)(uint64_t cluster_base, int state);
    uint32_t (*get_state)(uint64_t cluster_base);
    uint32_t (*get_target_state)(uint64_t cluster_base);
};

extern const struct cpufreq_hw_config s5l8960x_config, t7000_config, t7001_config, s800x_config,
    t8010_config, t8011_config, t8012_config, t8015_config;

extern uint32_t board_id;

void cpufreq_cmd(const char *cmd, char *args);

/* Generic functions */
uint64_t get_frequency_for_state(int state);
int set_state(int state);
uint64_t get_state(void);
uint64_t get_frequency_for_state(int state);
uint64_t get_core_type_for_state(int state);
uint64_t bench(void);

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

/* SoC-specific Overclocking functions */
int apply_magic_s5l8960x(const struct cpufreq_hw_config *config);
int apply_magic_s800x(const struct cpufreq_hw_config *config);
int apply_magic_t8010(const struct cpufreq_hw_config *config);
int apply_magic_t8015(const struct cpufreq_hw_config *config);

/* SoC-specific Voltage functions */
uint32_t get_vcore_s5l8960x(uint64_t cluster_base, uint32_t state);
uint32_t get_vcore_s800x(uint64_t cluster_base, uint32_t state);
uint32_t get_vcore_t8010(uint64_t cluster_base, uint32_t state);
#endif