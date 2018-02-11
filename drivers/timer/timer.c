/*
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0)
 */

#include <compiler.h>
#include <dm.h>
#include <error.h>
#include <stdbool.h>
#include <stddef.h>
#include <time.h>
#include <drivers/timer.h>

#include <debug.h>

static uint32_t tick_interval = SECONDS(1);
static struct device *timer;

#define MAX_PERIODIC_ITEMS 5

static struct work_item periodic_work_items[MAX_PERIODIC_ITEMS];

int
timer_cancel_periodic(work_function fn __unused, void *param __unused)
{
	/* Walk the list and try to find a matching work item */
	for (size_t i = 0; i < MAX_PERIODIC_ITEMS; i++) {
		if (periodic_work_items[i].fn    == fn &&
		    periodic_work_items[i].param == param)
			periodic_work_items[i].fn = NULL;
	}
	return SUCCESS;
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
timer_run_periodic(work_function fn, void *param)
{
	bool queued = false;
	/* Find the first available index */
	/* Does not find duplicates */
	for (size_t i = 0; i < MAX_PERIODIC_ITEMS && !queued; i++) {
		if (periodic_work_items[i].fn != NULL)
			continue;
		periodic_work_items[i].fn = fn;
		periodic_work_items[i].param = param;
		queued = true;
		break;
	}
	
	if (!queued)
		error("Periodic work queue full");
	return SUCCESS;
}

void
timer_tick(void)
{
	/* Walk through periodic work items and add each to work queue */
	for (int i = 0; i < MAX_PERIODIC_ITEMS; i++) {
		if (periodic_work_items[i].fn == NULL)
			continue;
		queue_work(periodic_work_items[i].fn,
		           periodic_work_items[i].param);
	}
	timer_refresh();
}
