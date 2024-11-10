#include <errno.h>
#include <pongo.h>

#include "cpufreq_private.h"
#include "utils.h"

uint64_t get_frequency_for_state(int state)
{
    uint64_t freq = data.hw_config->get_frequency_for_state(data.hw_config->cluster_base, state);

    if (!freq)
        freq = BASE_CLOCK;

    return freq;
}

int set_state(int state)
{
    return data.hw_config->set_state(data.hw_config->cluster_base, state);
}

uint64_t get_state(void)
{
    return data.hw_config->get_state(data.hw_config->cluster_base);
}

uint64_t get_target_state(void)
{
    return data.hw_config->get_target_state(data.hw_config->cluster_base);
}

uint64_t get_core_type_for_state(int state)
{
    if (data.hw_config->get_core_type_for_state)
        return data.hw_config->get_core_type_for_state(data.hw_config->cluster_base, state);
    else
        return CORE_TYPE_UNKNOWN;
}

void cpufreq_show(const char *cmd, char *args)
{
    uint32_t state = get_state();
    uint32_t target = get_target_state();
    uint64_t freq = get_frequency_for_state(state);

    printf("Current state           : %u\n", state);
    printf("Current target state    : %u\n", target);
    printf("Current CPU frequency   : %llu Hz\n", freq);

    if (data.hw_config->get_core_type_for_state)
        printf("Current CPU type        : %s\n", core_type_array[get_core_type_for_state(state)]);

    if (data.hw_config->get_vcore)
        printf("Vcore                   : %u mV\n",
               data.hw_config->get_vcore(data.hw_config->cluster_base, state));

    if ((state > data.max_nonboost_pstate) && (state <= data.max_configured_pstate))
        printf("Boost state\n");
}

void cpufreq_dump(const char *cmd, char *args)
{
    printf("Available states:\n");
    for (uint32_t i = 1; i <= data.max_configured_pstate; i++) {
        printf("CPU state       : %u\n", i);
        printf("CPU frequency   : %llu Hz\n", get_frequency_for_state(i));
        if (data.hw_config->get_core_type_for_state)
            printf("CPU type        : %s\n", core_type_array[get_core_type_for_state(i)]);

        if (data.hw_config->get_vcore)
            printf("Vcore           : %u mV\n",
                   data.hw_config->get_vcore(data.hw_config->cluster_base, i));

        if ((i > data.max_nonboost_pstate) && (i <= data.max_configured_pstate))
            printf("Boost state\n");

        printf("\n");
    }
}

void cpufreq_set(const char *cmd, char *args)
{
    if (!*args) {
        printf("usage: cpufreq set [state]\n");
        printf("\nState is a number from 0 to a device-dependent maximum value\n");
        printf("Run `cpufreq dump` to get more information.\n");
        return;
    }

    uint64_t state = strtoull(args, NULL, 0);
    if (!state || (uint32_t)state > data.max_configured_pstate) {
        printf("Invalid pstate specified. Max pstate is %u\n", data.max_configured_pstate);
        return;
    }

    set_state(state);
}

void cpufreq_unlock(const char *cmd, char *args)
{
    if (data.max_configured_pstate == data.max_nonboost_pstate)
        return;

    for (uint32_t i = (data.max_nonboost_pstate + 1); i <= data.max_configured_pstate; i++) {
        uint64_t addr = 0;

        switch (socnum) {
            case 0x8000:
            case 0x8001:
            case 0x8003:
                addr = data.hw_config->cluster_base + CLUSTER_PSINFO2_S8000(i);
                break;
            case 0x8010:
            case 0x8011:
            case 0x8012:
                addr = data.hw_config->cluster_base + CLUSTER_PSINFO2_T8010(i);
                ;
                break;
            case 0x8015:
                addr = data.hw_config->cluster_base + CLUSTER_PSINFO2_T8015(i);
                break;
        }

        if (!addr) {
            printf("cpufreq: Don't know how to unlock boost state on this SoC\n");
            return;
        }

        mask64(addr, CLUSTER_PSINFO_MAX_LOAD, FIELD_PREP(CLUSTER_PSINFO_MAX_LOAD, 15));
    }
}

void cpufreq_magic(const char *cmd, char *args)
{
    if (data.hw_config->apply_magic)
        set_state(data.hw_config->apply_magic(data.hw_config));
    else
        printf("cpufreq: No magic available for this SoC\n");
}

void cpufreq_bench(const char *cmd, char *args)
{
    uint64_t hz = bench();

    printf("CPU frequency: %lld Hz\n", hz);
}

#define CPUFREQ_COMMAND(_name, _desc, _cb)                                                         \
    {                                                                                              \
        .name = _name, .desc = _desc, .cb = _cb                                                    \
    }
void cpufreq_help(const char *cmd, char *args);

static struct cpufreq_command command_table[] = {
    CPUFREQ_COMMAND("help", "Show usage", cpufreq_help),
    CPUFREQ_COMMAND("dump", "Dump available CPU states", cpufreq_dump),
    CPUFREQ_COMMAND("set", "Set CPU state", cpufreq_set),
    CPUFREQ_COMMAND("show", "Get current CPU information", cpufreq_show),
    CPUFREQ_COMMAND("bench", "Measure CPU frequency", cpufreq_bench),
    CPUFREQ_COMMAND("unlock", "Unlock boost states (Device may become unstable)", cpufreq_unlock),
    CPUFREQ_COMMAND("magic", "Apply magic (DANGEROUS)", cpufreq_magic),
};

void cpufreq_help(const char *cmd, char *args)
{
    printf("cpufreq usage: cpufreq [subcommand] <subcommand options>\nsubcommands:\n");
    for (int i = 0; i < sizeof(command_table) / sizeof(struct cpufreq_command); i++) {
        if (command_table[i].name) {
            printf("%16s | %s\n", command_table[i].name,
                   command_table[i].desc ? command_table[i].desc : "no description");
        }
    }
}

void cpufreq_cmd(const char *cmd, char *args)
{
    char *arguments = command_tokenize(args, 0x1ff - (args - cmd));
    struct cpufreq_command *fallback_cmd = NULL;
    if (arguments) {
        for (int i = 0; i < sizeof(command_table) / sizeof(struct cpufreq_command); i++) {
            if (command_table[i].name && !strcmp("help", command_table[i].name)) {
                fallback_cmd = &command_table[i];
            }
            if (command_table[i].name && !strcmp(args, command_table[i].name)) {
                command_table[i].cb(args, arguments);
                return;
            }
        }
        if (*args)
            printf("cpufreq: invalid command %s\n", args);
        if (fallback_cmd)
            return fallback_cmd->cb(cmd, arguments);
    }
}