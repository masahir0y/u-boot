/*
 * Copyright (C) 2016 Socionext Inc.
 *   Author: Masahiro Yamada <yamada.masahiro@socionext.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include "clk-uniphier.h"

#define UNIPHIER_SLD3_SYS_CLK_STDMAC(id)				\
	UNIPHIER_CLK_GATE((id), 0x2104, 10)

#define UNIPHIER_LD11_SYS_CLK_STDMAC(id)				\
	UNIPHIER_CLK_GATE((id), 0x210c, 8)

#define UNIPHIER_PRO4_SYS_CLK_GIO(id)					\
	UNIPHIER_CLK_GATE((id), 0x2104, 6)

#define UNIPHIER_PRO4_SYS_CLK_USB3(id, ch)				\
	UNIPHIER_CLK_GATE((id), 0x2104, 16 + (ch))

const struct uniphier_clk_gate_data uniphier_pxs2_sys_clk_gate[] = {
	UNIPHIER_SLD3_SYS_CLK_STDMAC(8),
	UNIPHIER_PRO4_SYS_CLK_GIO(12),		/* Pro4, Pro5 */
	UNIPHIER_PRO4_SYS_CLK_USB3(14, 0),	/* Pro4, Pro5, PXs2 */
	UNIPHIER_PRO4_SYS_CLK_USB3(15, 1),
	UNIPHIER_CLK_GATE(16, 0x2104, 19),	/* PXs2: usb30-phy */
	UNIPHIER_CLK_GATE(20, 0x2104, 20),	/* PXs2: usb31-phy */
	UNIPHIER_CLK_END
};

const struct uniphier_clk_data uniphier_pxs2_sys_clk_data = {
	.gate = uniphier_pxs2_sys_clk_gate,
};

const struct uniphier_clk_gate_data uniphier_ld20_sys_clk_gate[] = {
	UNIPHIER_LD11_SYS_CLK_STDMAC(8),
	UNIPHIER_CLK_GATE(14, 0x210c, 14),	/* LD20: usb30 */
	UNIPHIER_CLK_GATE(16, 0x210c, 12),	/* LD20: usb30-phy0 */
	UNIPHIER_CLK_GATE(17, 0x210c, 13),	/* LD20: usb30-phy1 */
	UNIPHIER_CLK_END
};

const struct uniphier_clk_data uniphier_ld20_sys_clk_data = {
	.gate = uniphier_ld20_sys_clk_gate,
};
