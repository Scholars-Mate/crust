/*
 * Copyright © 2013-2014, ARM Limited and Contributors. All rights reserved.
 * Copyright © 2017-2018 The Crust Firmware Authors.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MMIO_H
#define MMIO_H

#include <stdint.h>

static inline uint32_t
mmio_read32(uintptr_t addr)
{
	return *(volatile uint32_t *)addr;
}

static inline void
mmio_write32(uintptr_t addr, uint32_t value)
{
	*(volatile uint32_t *)addr = value;
}

static inline void
mmio_clearbits32(uintptr_t addr, uint32_t clear)
{
	mmio_write32(addr, mmio_read32(addr) & ~clear);
}

static inline void
mmio_clearsetbits32(uintptr_t addr, uint32_t clear, uint32_t set)
{
	mmio_write32(addr, (mmio_read32(addr) & ~clear) | set);
}

static inline void
mmio_setbits32(uintptr_t addr, uint32_t set)
{
	mmio_write32(addr, mmio_read32(addr) | set);
}

#endif /* MMIO_H */
