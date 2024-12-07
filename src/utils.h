#ifndef _UTILS_H_
#define _UTILS_H_
#include <pongo.h>
#include <stdint.h>

#define BIT(x)                 (1UL << (x))
#define MASK(x)                (BIT(x) - 1)
#define GENMASK(msb, lsb)      ((BIT((msb + 1) - (lsb)) - 1) << (lsb))
#define _FIELD_LSB(field)      ((field) & ~(field - 1))
#define FIELD_PREP(field, val) ((val) * (_FIELD_LSB(field)))
#define FIELD_GET(field, val)  (((val) & (field)) / _FIELD_LSB(field))

#define _concat(a, _1, b, ...) a##b

#define _sr_tkn_S(_0, _1, op0, op1, CRn, CRm, op2) s##op0##_##op1##_c##CRn##_c##CRm##_##op2

#define _sr_tkn(a) a

#define sr_tkn(...) _concat(_sr_tkn, __VA_ARGS__, )(__VA_ARGS__)

#define __mrs(reg)                                                                                 \
    ({                                                                                             \
        uint64_t val;                                                                              \
        __asm__ volatile("mrs\t%0, " #reg : "=r"(val));                                            \
        val;                                                                                       \
    })
#define _mrs(reg) __mrs(reg)

#define __msr(reg, val)                                                                            \
    ({                                                                                             \
        uint64_t __val = (u64)val;                                                                 \
        __asm__ volatile("msr\t" #reg ", %0" : : "r"(__val));                                      \
    })
#define _msr(reg, val) __msr(reg, val)

#define mrs(reg)      _mrs(sr_tkn(reg))
#define msr(reg, val) _msr(sr_tkn(reg), val)

static inline uint64_t read64(uint64_t addr)
{
    return *(volatile uint64_t *)addr;
}

static inline void write64(uint64_t addr, uint64_t val)
{
    *(volatile uint64_t *)addr = val;
}

static inline uint32_t read32(uint64_t addr)
{
    return *(volatile uint32_t *)addr;
}

static inline void write32(uint64_t addr, uint32_t val)
{
    *(volatile uint32_t *)addr = val;
}

static inline void clear64(uint64_t addr, uint64_t val)
{
    uint64_t reg = read64(addr);
    write64(addr, addr & (~val));
}

static inline void set64(uint64_t addr, uint64_t val)
{
    uint64_t reg = read64(addr);
    write64(addr, addr | val);
}

static inline void udelay(uint32_t d)
{
    uint64_t delay = ((uint64_t)d) * mrs(CNTFRQ_EL0) / 1000000;
    uint64_t val = mrs(CNTPCT_EL0);
    while ((mrs(CNTPCT_EL0) - val) < delay)
        ;
    __asm__("isb");
}

static inline int poll32(uint64_t addr, uint64_t mask, uint64_t target, uint64_t timeout)
{
    while (--timeout > 0) {
        uint32_t value = read32(addr) & mask;
        if (value == target)
            return 0;
    }
    udelay(1);

    return -1;
}

static inline int poll64(uint64_t addr, uint64_t mask, uint64_t target, uint64_t timeout)
{
    while (--timeout > 0) {
        uint32_t value = read32(addr) & mask;
        if (value == target)
            return 0;
    }
    udelay(1);

    return -1;
}

static inline uint64_t mask64(uint64_t addr, uint64_t clear, uint64_t set)
{
    uint64_t reg = read64(addr);
    reg &= ~clear;
    reg |= set;
    write64(addr, reg);
    return reg;
}

#endif