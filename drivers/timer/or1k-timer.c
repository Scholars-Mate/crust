/*
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0)
 */

#include <compiler.h>
#include <debug.h>
#include <dm.h>
#include <error.h>
#include <mmio.h>
#include <spr.h>
#include <stddef.h>
#include <drivers/timer.h>
#include <drivers/timer/or1k-timer.h>

#define TICKS_PER_USEC 24

void
or1k_timer_clear(void)
{
	uint32_t reg;

	reg = mfspr(SPR_TICK_TTMR_ADDR);
	reg = SPR_TICK_TTMR_IP_SET(reg, 0);
	reg = SPR_TICK_TTMR_IE_SET(reg, 0);
	reg = SPR_TICK_TTMR_MODE_SET(reg, SPR_TICK_TTMR_MODE_DISABLE);
	mtspr(SPR_TICK_TTMR_ADDR, reg);
}

static int
or1k_timer_get_timeout(struct device *dev __unused, uint32_t *timeout)
{
	uint32_t mode = mfspr(SPR_TICK_TTMR_ADDR);

	assert(timeout);

	if (SPR_TICK_TTMR_MODE_GET(mode) == SPR_TICK_TTMR_MODE_DISABLE) {
		*timeout = 0;
	} else {
		uint32_t count  = mfspr(SPR_TICK_TTCR_ADDR);
		uint32_t period = SPR_TICK_TTMR_TP_GET(mode);
		*timeout = (period - count) / TICKS_PER_USEC;
	}

	return SUCCESS;
}

static int
or1k_timer_probe(struct device *dev)
{
	int err;

	/* Disable the timer and its interrupt. */
	or1k_timer_clear();

	/* Unmask the interrupt at the CPU level. */
	mtspr(SPR_SYS_SR_ADDR, SPR_SYS_SR_TEE_SET(mfspr(SPR_SYS_SR_ADDR), 1));

	/* Register this device with the timer framework. */
	if ((err = timer_device_register(dev)))
		return err;

	return SUCCESS;
}

static int
or1k_timer_set_timeout(struct device *dev __unused, uint32_t timeout)
{
	uint32_t reg;

	if (timeout >= BIT(SPR_TICK_TTMR_TP_BITS) / TICKS_PER_USEC)
		return ERANGE;

	/* Reset the counter to zero. */
	mtspr(SPR_TICK_TTCR_ADDR, 0);

	/* Enable the interrupt and start counting. */
	reg = mfspr(SPR_TICK_TTMR_ADDR);
	reg = SPR_TICK_TTMR_TP_SET(reg, timeout * TICKS_PER_USEC);
	reg = SPR_TICK_TTMR_IE_SET(reg, 1);
	reg = SPR_TICK_TTMR_MODE_SET(reg, SPR_TICK_TTMR_MODE_STOP);
	mtspr(SPR_TICK_TTMR_ADDR, reg);

	return SUCCESS;
}

static const struct timer_driver_ops or1k_timer_driver_ops = {
	.get_timeout = or1k_timer_get_timeout,
	.set_timeout = or1k_timer_set_timeout,
};

const struct driver or1k_timer_driver = {
	.name  = "or1k-timer",
	.class = DM_CLASS_TIMER,
	.probe = or1k_timer_probe,
	.ops   = &or1k_timer_driver_ops,
};
