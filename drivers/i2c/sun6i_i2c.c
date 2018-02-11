/*
 * Copyright © 2017-2018 The Crust Firmware Authors.
 * SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0)
 */

#include <dm.h>
#include <error.h>
#include <drivers/i2c/sun6i_i2c.h>
#include <drivers/i2c.h>

#define TWI_ADDR  0x0000
#define TWI_XADDR 0x0004
#define TWI_DATA  0x0008
#define TWI_CNTR  0x000c
#define TWI_STAT  0x0010
#define TWI_CCR   0x0014
#define TWI_SRST  0x0018
#define TWI_EFR   0x001c
#define TWI_LCR   0x0020

static struct device *i2c_dev;

static int
sun6i_i2c_probe(struct device *dev)
{
	i2c_dev = dev;
	return SUCCESS;
}

const struct driver sun6i_i2c_driver = {
	.name  = "sun6i_i2c_driver",
	.class = DM_CLASS_I2C,
	.probe = sun6i_i2c_probe,
	.ops   = 0,
};
