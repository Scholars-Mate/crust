/*
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0)
 */

#include <compiler.h>
#include <dm.h>
#include <error.h>
#include <stddef.h>
#include <time.h>
#include <drivers/timer.h>

static uint32_t tick_interval = SECONDS(1);
static struct device *timer;

int
timer_cancel_periodic(work_function fn __unused, void *param __unused)
{
	return ENOTSUP;
}

int
timer_device_register(struct device *dev)
{
	if (timer != NULL)
		return EEXIST;

	timer = dev;

	return SUCCESS;
}

int
timer_get_timeout(uint32_t *timeout)
{
	if (timer == NULL)
		return ENODEV;

	return TIMER_OPS(timer)->get_timeout(timer, timeout);
}

int
timer_refresh(void)
{
	uint32_t timeout;

	if (timer == NULL)
		return ENODEV;

	timeout = tick_interval;
	TIMER_OPS(timer)->set_timeout(timer, timeout);

	return SUCCESS;
}

int
timer_run_delayed(work_function fn __unused, void *param __unused,
                  uint32_t delay __unused)
{
	return ENOTSUP;
}

int
timer_run_periodic(work_function fn __unused, void *param __unused)
{
	return ENOTSUP;
}

void
timer_tick(void)
{
	timer_refresh();
}
