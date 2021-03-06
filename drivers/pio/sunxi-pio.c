/*
 * Copyright © 2017-2018 The Crust Firmware Authors.
 * SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0)
 */

#include <dm.h>
#include <error.h>
#include <mmio.h>
#include <stdbool.h>
#include <stddef.h>
#include <util.h>
#include <drivers/clock.h>
#include <drivers/irqchip.h>
#include <drivers/pio.h>
#include <drivers/pio/sunxi-pio.h>

#define MAX_PORTS      8

#define PIN_INDEX(pin) ((pin) & BITMASK(0, 5))
#define PIN_PORT(pin)  ((pin) >> 5)

#define PIN_MODE(mode) ((mode) & BITMASK(0, 3))

#define CONFIG_REG(pin) \
	(0x0000 + PIN_PORT(pin) * 0x24 + (PIN_INDEX(pin) >> 3) * 4)
#define DATA_REG(pin) \
	(0x0010 + PIN_PORT(pin) * 0x24)

#define CONFIG_OFFSET(pin)    ((PIN_INDEX(pin) & BITMASK(0, 3)) * 4)

#define INT_CONTROL_REG(port) (0x0210 + (port) * 0x20)
#define INT_STATUS_REG(port)  (0x0214 + (port) * 0x20)

static bool
sunxi_pio_read_pin(struct device *dev, uint8_t pin)
{
	return mmio_read32(dev->regs + DATA_REG(pin)) & BIT(PIN_INDEX(pin));
}

static int
sunxi_pio_set_mode(struct device *dev, uint8_t pin, uint8_t mode)
{
	/* Verify port exists. */
	if (!(dev->drvdata & BIT(PIN_PORT(pin))))
		return ENODEV;

	mmio_clearsetbits32(dev->regs + CONFIG_REG(pin),
	                    BITMASK(CONFIG_OFFSET(pin), 3),
	                    PIN_MODE(mode) << CONFIG_OFFSET(pin));

	return SUCCESS;
}

static int
sunxi_pio_write_pin(struct device *dev, uint8_t pin, bool val)
{
	/* Set pin to specified val. */
	mmio_clearsetbits32(dev->regs + DATA_REG(pin),
	                    BIT(PIN_INDEX(pin)),
	                    val << PIN_INDEX(pin));

	return SUCCESS;
}

static const struct pio_driver_ops sunxi_pio_driver_ops = {
	.read_pin  = sunxi_pio_read_pin,
	.set_mode  = sunxi_pio_set_mode,
	.write_pin = sunxi_pio_write_pin,
};

static int
sunxi_pio_probe(struct device *dev)
{
	int err;

	if ((err = clock_enable(dev)))
		return err;

	/* Disable and clear all IRQs. */
	for (size_t port = 0; port < MAX_PORTS; ++port) {
		if (dev->drvdata & BIT(port)) {
			mmio_write32(dev->regs + INT_CONTROL_REG(port), 0x0);
			mmio_write32(dev->regs + INT_STATUS_REG(port), ~0);
		}
	}

	return SUCCESS;
}

const struct driver sunxi_pio_driver = {
	.name  = "sunxi-pio",
	.class = DM_CLASS_PIO,
	.probe = sunxi_pio_probe,
	.ops   = &sunxi_pio_driver_ops,
};
