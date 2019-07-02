// SPDX-License-Identifier: GPL-2.0-only
//
// Copyright (C) 2019 Socionext Inc.

#include <linux/delay.h>

#include "../init.h"
#include "../sc64-regs.h"
#include "pll.h"

/* PLL type: SSC */
#define SC_CPLLCTRL	0x1400	/* CPU/ARM */
#define SC_SPLLCTRL	0x1410	/* misc */
#define SC_GPLLCTRL	0x1420
#define SC_AIPLLCTRL	0x1430
#define SC_DPLLCTRL	0x1440
#define SC_EPLLCTRL	0x1450

/* PLL type: VPLL27 */
#define SC_VPLL27CTRL	0x1500

void uniphier_nx1_pll_init(void)
{
	uniphier_ld20_sscpll_init(SC_CPLLCTRL, UNIPHIER_PLL_FREQ_DEFAULT, 0, 4);
	/* do nothing for SPLL */
	uniphier_ld20_sscpll_init(SC_GPLLCTRL, UNIPHIER_PLL_FREQ_DEFAULT, 0, 4);
	uniphier_ld20_sscpll_init(SC_AIPLLCTRL, UNIPHIER_PLL_FREQ_DEFAULT, 0, 2);
	uniphier_ld20_sscpll_init(SC_DPLLCTRL, UNIPHIER_PLL_FREQ_DEFAULT, 0, 2);
	uniphier_ld20_sscpll_init(SC_EPLLCTRL, UNIPHIER_PLL_FREQ_DEFAULT, 0, 2);

	mdelay(1);

	uniphier_ld20_sscpll_ssc_en(SC_CPLLCTRL);
	uniphier_ld20_sscpll_ssc_en(SC_SPLLCTRL);
	uniphier_ld20_sscpll_ssc_en(SC_GPLLCTRL);
	uniphier_ld20_sscpll_ssc_en(SC_AIPLLCTRL);
	uniphier_ld20_sscpll_ssc_en(SC_DPLLCTRL);
	uniphier_ld20_sscpll_ssc_en(SC_EPLLCTRL);

	uniphier_ld20_vpll27_init(SC_VPLL27CTRL);
}
