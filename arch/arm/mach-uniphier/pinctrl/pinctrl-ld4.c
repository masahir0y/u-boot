/*
 * Copyright (C) 2011-2015 Masahiro Yamada <yamada.masahiro@socionext.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <linux/io.h>

#include "../init.h"
#include "../sg-regs.h"

void uniphier_ld4_pin_init(void)
{
	/* Comment format:    PAD Name -> Function Name */
#if 0
	/* Ether */
	sg_set_pinsel(32, 0, 8, 4); /* RMII_RXD0   -> RMII_RXD0 */
	sg_set_pinsel(33, 0, 8, 4); /* RMII_RXD1   -> RMII_RXD1 */
	sg_set_pinsel(34, 0, 8, 4); /* RMII_CRS_DV -> RMII_CRS_DV */
	sg_set_pinsel(35, 0, 8, 4); /* RMII_RXER   -> RMII_RXER */
	sg_set_pinsel(36, 0, 8, 4); /* RMII_REFCLK -> RMII_REFCLK */
	sg_set_pinsel(37, 0, 8, 4); /* RMII_TXD0   -> RMII_TXD0 */
	sg_set_pinsel(38, 0, 8, 4); /* RMII_TXD1   -> RMII_TXD1 */
	sg_set_pinsel(39, 0, 8, 4); /* RMII_TXEN   -> RMII_TXEN */
	sg_set_pinsel(40, 0, 8, 4); /* MDC         -> MDC */
	sg_set_pinsel(41, 0, 8, 4); /* MDIO        -> MDIO */
	sg_set_pinsel(42, 0, 8, 4); /* MDIO_INTL   -> MDIO_INTL */
	sg_set_pinsel(43, 0, 8, 4); /* PHYRSTL     -> PHYRSTL */

#if 1 /* RMII mode off */
	sg_set_pinsel(136, 4, 8, 4); /* PORT01   -> MII_COL */
	sg_set_pinsel(137, 4, 8, 4); /* PORT02   -> MII_CRS */
	sg_set_pinsel(138, 4, 8, 4); /* PORT03   -> MII_RXCLK */
	sg_set_pinsel(139, 4, 8, 4); /* PORT04   -> MII_RXD2 */
	sg_set_pinsel(140, 4, 8, 4); /* PORT05   -> MII_RXD3 */
	sg_set_pinsel(141, 4, 8, 4); /* PORT06   -> MII_TXD2 */
	sg_set_pinsel(142, 4, 8, 4); /* PORT07   -> MII_TXD3 */
	writel(0, SG_ETPINMODE);
#else
	writel(1, SG_ETPINMODE);
#endif
}
