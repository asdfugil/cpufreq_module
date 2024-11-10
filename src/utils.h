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

static inline int poll32(uint64_t addr, uint64_t mask, uint64_t target, uint64_t timeout)
{
    while (--timeout > 0) {
        uint32_t value = read32(addr) & mask;
        if (value == target)
            return 0;
    }
    usleep(1);

    return -1;
}

#endif