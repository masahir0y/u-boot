// SPDX-License-Identifier: GPL-2.0+
//
// Copyright (C) 2019 Socionext Inc.
//   Author: Masahiro Yamada <yamada.masahiro@socionext.com>

#include <common.h>
#include <dm.h>
#include <dm/pinctrl.h>

#include "pinctrl-uniphier.h"

static const unsigned int emmc_pins[] = {10, 85, 86, 87, 88, 89, 90, 91};
static const int emmc_muxvals[] = {0, 0, 0, 0, 0, 0, 0, 0};
static const unsigned int emmc_dat8_pins[] = {92, 93, 94, 95};
static const int emmc_dat8_muxvals[] = {0, 0, 0, 0};
static const unsigned int ether_rgmii_pins[] = {11, 12, 13, 14, 15, 16, 17, 18,
						19, 20, 21, 22, 23, 24, 25, 26};
static const int ether_rgmii_muxvals[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					  0, 0, 0, 0};
static const unsigned int ether_rmii_pins[] = {11, 12, 13, 14, 15, 16, 17, 18,
					       20, 22, 23, 26};
static const int ether_rmii_muxvals[] = {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1};
static const unsigned int i2c0_pins[] = {50, 51};
static const int i2c0_muxvals[] = {0, 0};
static const unsigned int i2c1_pins[] = {52, 53};
static const int i2c1_muxvals[] = {0, 0};
static const unsigned int i2c2_pins[] = {54, 55};
static const int i2c2_muxvals[] = {0, 0};
static const unsigned int i2c3_pins[] = {56, 57};
static const int i2c3_muxvals[] = {0, 0};
static const unsigned int sd_pins[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
static const int sd_muxvals[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
static const unsigned int spi0_pins[] = {42, 43, 44, 45};
static const int spi0_muxvals[] = {0, 0, 0, 0};
static const unsigned int spi1_pins[] = {46, 47, 48, 49};
static const int spi1_muxvals[] = {0, 0, 0, 0};
static const unsigned int uart0_pins[] = {27, 28};
static const int uart0_muxvals[] = {0, 0};
static const unsigned int uart1_pins[] = {29, 30};
static const int uart1_muxvals[] = {0, 0};
static const unsigned int uart2_pins[] = {36, 37};
static const int uart2_muxvals[] = {0, 0};
static const unsigned int uart3_pins[] = {40, 41};
static const int uart3_muxvals[] = {0, 0};
static const unsigned int usb0_pins[] = {78, 79};
static const int usb0_muxvals[] = {0, 0};
static const unsigned int usb1_pins[] = {80, 81};
static const int usb1_muxvals[] = {0, 0};

static const struct uniphier_pinctrl_group uniphier_nx1_groups[] = {
	UNIPHIER_PINCTRL_GROUP(emmc),
	UNIPHIER_PINCTRL_GROUP(emmc_dat8),
	UNIPHIER_PINCTRL_GROUP(ether_rgmii),
	UNIPHIER_PINCTRL_GROUP(ether_rmii),
	UNIPHIER_PINCTRL_GROUP(i2c0),
	UNIPHIER_PINCTRL_GROUP(i2c1),
	UNIPHIER_PINCTRL_GROUP(i2c2),
	UNIPHIER_PINCTRL_GROUP(i2c3),
	UNIPHIER_PINCTRL_GROUP(sd),
	UNIPHIER_PINCTRL_GROUP(spi0),
	UNIPHIER_PINCTRL_GROUP(spi1),
	UNIPHIER_PINCTRL_GROUP(uart0),
	UNIPHIER_PINCTRL_GROUP(uart1),
	UNIPHIER_PINCTRL_GROUP(uart2),
	UNIPHIER_PINCTRL_GROUP(uart3),
	UNIPHIER_PINCTRL_GROUP(usb0),
	UNIPHIER_PINCTRL_GROUP(usb1),
};

static const char * const uniphier_nx1_functions[] = {
	UNIPHIER_PINMUX_FUNCTION(emmc),
	UNIPHIER_PINMUX_FUNCTION(ether_rgmii),
	UNIPHIER_PINMUX_FUNCTION(ether_rmii),
	UNIPHIER_PINMUX_FUNCTION(i2c0),
	UNIPHIER_PINMUX_FUNCTION(i2c1),
	UNIPHIER_PINMUX_FUNCTION(i2c2),
	UNIPHIER_PINMUX_FUNCTION(i2c3),
	UNIPHIER_PINMUX_FUNCTION(sd),
	UNIPHIER_PINMUX_FUNCTION(spi0),
	UNIPHIER_PINMUX_FUNCTION(spi1),
	UNIPHIER_PINMUX_FUNCTION(uart0),
	UNIPHIER_PINMUX_FUNCTION(uart1),
	UNIPHIER_PINMUX_FUNCTION(uart2),
	UNIPHIER_PINMUX_FUNCTION(uart3),
	UNIPHIER_PINMUX_FUNCTION(usb0),
	UNIPHIER_PINMUX_FUNCTION(usb1),
};

static struct uniphier_pinctrl_socdata uniphier_nx1_pinctrl_socdata = {
	.groups = uniphier_nx1_groups,
	.groups_count = ARRAY_SIZE(uniphier_nx1_groups),
	.functions = uniphier_nx1_functions,
	.functions_count = ARRAY_SIZE(uniphier_nx1_functions),
	.caps = UNIPHIER_PINCTRL_CAPS_PUPD_SIMPLE |
		UNIPHIER_PINCTRL_CAPS_PERPIN_IECTRL,
};

static int uniphier_nx1_pinctrl_probe(struct udevice *dev)
{
	return uniphier_pinctrl_probe(dev, &uniphier_nx1_pinctrl_socdata);
}

static const struct udevice_id uniphier_nx1_pinctrl_match[] = {
	{ .compatible = "socionext,uniphier-nx1-pinctrl" },
	{ /* sentinel */ }
};

U_BOOT_DRIVER(uniphier_nx1_pinctrl) = {
	.name = "uniphier-nx1-pinctrl",
	.id = UCLASS_PINCTRL,
	.of_match = of_match_ptr(uniphier_nx1_pinctrl_match),
	.probe = uniphier_nx1_pinctrl_probe,
	.priv_auto_alloc_size = sizeof(struct uniphier_pinctrl_priv),
	.ops = &uniphier_pinctrl_ops,
};
