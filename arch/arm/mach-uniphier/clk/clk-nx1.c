// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2019 Socionext Inc.
 */

#include <linux/io.h>

#include "../init.h"
#include "../sc64-regs.h"

#define SDCTRL_EMMC_HW_RESET	0x19810280

void uniphier_nx1_clk_init(void)
{
	/* TODO: use "mmc-pwrseq-emmc" */
	writel(1, SDCTRL_EMMC_HW_RESET);
}
