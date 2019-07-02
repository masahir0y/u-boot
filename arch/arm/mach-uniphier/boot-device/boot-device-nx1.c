// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2019 Socionext Inc.
 *   Author: Masahiro Yamada <yamada.masahiro@socionext.com>
 */

#include <common.h>
#include <spl.h>
#include <linux/io.h>
#include <linux/kernel.h>

#include "../sg-regs.h"
#include "boot-device.h"

const struct uniphier_boot_device uniphier_nx1_boot_device_table[] = {
	{BOOT_DEVICE_MMC1, "eMMC (Boot partition auto detect)"},
	{BOOT_DEVICE_MMC1, "eMMC (Boot partition 1)"},
	{BOOT_DEVICE_MMC1, "eMMC (Boot partition 2)"},
	{BOOT_DEVICE_NOR,  "NOR"},
	{BOOT_DEVICE_NONE, "Reserved"},
	{BOOT_DEVICE_NONE, "Reserved"},
	{BOOT_DEVICE_NOR,  "NOR"},
	{BOOT_DEVICE_NOR,  "NOR"},
};

const unsigned uniphier_nx1_boot_device_count =
				ARRAY_SIZE(uniphier_nx1_boot_device_table);

int uniphier_nx1_boot_device_is_sd(void)
{
	return !!(readl(sg_base + SG_PINMON0) & BIT(8));
}
