/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * UniPhier UMC (Universal Memory Controller) registers for NX1
 */

#ifndef UNIPHIER_UMC_MEMCONF_H
#define UNIPHIER_UMC_MEMCONF_H

#include <linux/bitops.h>

#define UMC_MEMCONFCH		0x010

#define UMC_MEMCONFCH_SZ_MASK		GENMASK(11, 8)
#define UMC_MEMCONFCH_SZ_1G		(0x8 << 8)
#define UMC_MEMCONFCH_SZ_2G		(0x9 << 8)
#define UMC_MEMCONFCH_SZ_4G		(0xa << 8)

#endif /* UNIPHIER_UMC_MEMCONF_H */
