/*
 * UniPhier Specific Glue Layer for DWC3
 *
 * Copyright (C) 2016-2017 Socionext Inc.
 *   Author: Masahiro Yamada <yamada.masahiro@socionext.com>
 *   Author: Dai Okamura <okamura.dai@socionext.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <dm.h>
#include <linux/bitops.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/io.h>
#include <linux/sizes.h>

#define UNIPHIER_PRO4_DWC3_RESET	0x040
#define   UNIPHIER_PRO4_DWC3_RESET_XIOMMU	BIT(5)
#define   UNIPHIER_PRO4_DWC3_RESET_XLINK	BIT(4)
#define   UNIPHIER_PRO4_DWC3_RESET_PHY_SS	BIT(2)
#define UNIPHIER_PRO4_DWC3_U3PHY_TESTI	0x010
#define UNIPHIER_PRO4_DWC3_U3PHY_TESTO	0x014

#define UNIPHIER_PRO5_DWC3_RESET	0x000
#define   UNIPHIER_PRO5_DWC3_RESET_PHY_S1	BIT(17)
#define   UNIPHIER_PRO5_DWC3_RESET_PHY_S0	BIT(16)
#define   UNIPHIER_PRO5_DWC3_RESET_XLINK	BIT(15)
#define   UNIPHIER_PRO5_DWC3_RESET_XIOMMU	BIT(14)
#define UNIPHIER_PRO5_DWC3_HSPHY_PARAM2_REG	0x288
#define UNIPHIER_PRO5_DWC3_SSPHY_PARAM2_REG	0x384
#define UNIPHIER_PRO5_DWC3_SSPHY_PARAM3_REG	0x388

/* PXs2/LD6b/LD20/PXs3 have same DWC3 control register mapping */
#define UNIPHIER_PXS2_DWC3_RESET	0x000
#define   UNIPHIER_PXS2_DWC3_RESET_XLINK	BIT(15)
#define UNIPHIER_PXS2_DWC3_VBUS_CONTROL(i)	(0x100 + (i) * 0x10)
#define   UNIPHIER_PXS2_DWC3_VBUS_CONTROL_VBUS_BIT_EN	BIT(3)
#define   UNIPHIER_PXS2_DWC3_VBUS_CONTROL_VBUS_BIT_ONOFF	BIT(4)
#define UNIPHIER_PXS2_DWC3_U2PHY_CFG0(i)	(0x200 + (i) * 0x10)
#define   UNIPHIER_PXS2_DWC3_U2PHY_CFG0_HS_SWING_SEL_SHIFT	16
#define   UNIPHIER_PXS2_DWC3_U2PHY_CFG0_HS_SWING_SEL_MASK	\
	  (0x3 << UNIPHIER_PXS2_DWC3_U2PHY_CFG0_HS_SWING_SEL_SHIFT)
#define UNIPHIER_PXS2_DWC3_U2PHY_CFG1(i)	(0x204 + (i) * 0x10)
#define UNIPHIER_PXS2_DWC3_U3PHY_TESTI(i)	(0x300 + (i) * 0x10)
#define UNIPHIER_PXS2_DWC3_U3PHY_TESTO(i)	(0x304 + (i) * 0x10)
#define UNIPHIER_PXS2_DWC3_HOST_CFG	0x400
#define   UNIPHIER_PXS2_DWC3_HOST_CFG_NUM_U3_SHIFT	11
#define   UNIPHIER_PXS2_DWC3_HOST_CFG_NUM_U2_SHIFT	8
#define   UNIPHIER_PXS2_DWC3_HOST_CFG_NUM_U_MASK	0x7

#define TESTI_DATA(x)	(((x) & 0xff) << 6)
#define TESTI_ADDR(x)	(((x) & 0x1f) << 1)
#define TESTI_WR_STROBE	BIT(0)

static void uniphier_pphy_test_io_pro4(void __iomem *regs, u32 data0)
{
	int i;
	void __iomem *vptr_i, *vptr_o;

	vptr_i = regs + UNIPHIER_PRO4_DWC3_U3PHY_TESTI;
	vptr_o = regs + UNIPHIER_PRO4_DWC3_U3PHY_TESTO;

	writel(data0 | 0, vptr_i);
	for (i = 0; i < 10; i++)
		readl(vptr_o);

	writel(data0 | 1, vptr_i);
	for (i = 0; i < 10; i++)
		readl(vptr_o);

	writel(data0 | 0, vptr_i);
	for (i = 0; i < 10; i++)
		readl(vptr_o);
}

static int uniphier_pro4_dwc3_init(void __iomem *regs)
{
	u32 tmp;

	/* set up SS-PHY */
	uniphier_pphy_test_io_pro4(regs, 0x206);
	uniphier_pphy_test_io_pro4(regs, 0x08e);
	uniphier_pphy_test_io_pro4(regs, 0x27c);
	uniphier_pphy_test_io_pro4(regs, 0x2b8);
	uniphier_pphy_test_io_pro4(regs, 0x102);
	uniphier_pphy_test_io_pro4(regs, 0x22a);
	uniphier_pphy_test_io_pro4(regs, 0x02c);
	uniphier_pphy_test_io_pro4(regs, 0x100);
	uniphier_pphy_test_io_pro4(regs, 0x09a);

	/* negate and release reset by XHCI LINK RESET */
	tmp = readl(regs + UNIPHIER_PRO4_DWC3_RESET);
	tmp &= ~UNIPHIER_PRO4_DWC3_RESET_XLINK;
	writel(tmp, regs + UNIPHIER_PRO4_DWC3_RESET);
	udelay(1000);
	tmp &= ~UNIPHIER_PRO4_DWC3_RESET_PHY_SS;
	tmp |= UNIPHIER_PRO4_DWC3_RESET_XIOMMU | UNIPHIER_PRO4_DWC3_RESET_XLINK;
	writel(tmp, regs + UNIPHIER_PRO4_DWC3_RESET);

	return 0;
}

static int uniphier_pro5_dwc3_init(void __iomem *regs)
{
	u32 tmp;

	/* set up PHY */
	/* SSPHY Reference Clock Enable for SS function */
	tmp = readl(regs + UNIPHIER_PRO5_DWC3_SSPHY_PARAM3_REG);
	tmp |= BIT(31);
	writel(tmp, regs + UNIPHIER_PRO5_DWC3_SSPHY_PARAM3_REG);

	/* HSPHY MPLL Frequency Multiplier Control, Frequency Select */
	tmp = readl(regs + UNIPHIER_PRO5_DWC3_HSPHY_PARAM2_REG);
	tmp &= ~(0x7f7f0000);
	tmp |= 0x7d310000;
	writel(tmp, regs + UNIPHIER_PRO5_DWC3_HSPHY_PARAM2_REG);
	/* SSPHY Loopback off */
	tmp = readl(regs + UNIPHIER_PRO5_DWC3_SSPHY_PARAM2_REG);
	tmp &= ~BIT(24);
	writel(tmp, regs + UNIPHIER_PRO5_DWC3_SSPHY_PARAM2_REG);

	/* negate and release reset by XHCI LINK RESET */
	tmp = readl(regs + UNIPHIER_PRO5_DWC3_RESET);
	tmp &= ~UNIPHIER_PRO5_DWC3_RESET_XLINK;
	writel(tmp, regs + UNIPHIER_PRO5_DWC3_RESET);
	udelay(1000);
	tmp &= ~(UNIPHIER_PRO5_DWC3_RESET_PHY_S1 |
		 UNIPHIER_PRO5_DWC3_RESET_PHY_S0);
	tmp |= UNIPHIER_PRO5_DWC3_RESET_XLINK | UNIPHIER_PRO5_DWC3_RESET_XIOMMU;
	writel(tmp, regs + UNIPHIER_PRO5_DWC3_RESET);

	return 0;
}

static void uniphier_pphy_test_io(void __iomem *regs, int port,
	u32 addr, u32 mask, u32 data)
{
	void __iomem *vptr_i, *vptr_o;

	vptr_i = regs + UNIPHIER_PXS2_DWC3_U3PHY_TESTI(port);
	vptr_o = regs + UNIPHIER_PXS2_DWC3_U3PHY_TESTO(port);

	u32 rd;
	u32 wd;

	wd = TESTI_ADDR(addr) | TESTI_DATA(1);
	writel(wd, vptr_i);
	readl(vptr_o);
	readl(vptr_o);

	rd = readl(vptr_o);
	wd = TESTI_ADDR(addr) | TESTI_DATA((rd & ~mask) | data);
	writel(wd, vptr_i);
	readl(vptr_o);
	readl(vptr_o);

	wd |= TESTI_WR_STROBE;
	writel(wd, vptr_i);
	readl(vptr_o);
	readl(vptr_o);

	wd &= ~TESTI_WR_STROBE;
	writel(wd, vptr_i);
	readl(vptr_o);
	readl(vptr_o);

	rd = readl(vptr_i);
	wd = rd | TESTI_ADDR(addr) | TESTI_DATA(1);
	writel(wd, vptr_i);
	readl(vptr_o);
	readl(vptr_o);
}

static int uniphier_pxs2_dwc3_init(void __iomem *regs)
{
	u32 tmp;
	int i;
	int ss_instances;

	/* get the number of SS port from the HW default value */
	tmp = readl(regs + UNIPHIER_PXS2_DWC3_HOST_CFG);
	ss_instances = (tmp >> UNIPHIER_PXS2_DWC3_HOST_CFG_NUM_U3_SHIFT)
		 & UNIPHIER_PXS2_DWC3_HOST_CFG_NUM_U_MASK;

	/* set up SS-PHY */
	for (i = 0; i < ss_instances; i++) {
		uniphier_pphy_test_io(regs, i,  7, 0xf, 0xa);
		uniphier_pphy_test_io(regs, i,  8, 0xf, 0x3);
		uniphier_pphy_test_io(regs, i,  9, 0xf, 0x5);
		uniphier_pphy_test_io(regs, i, 11, 0xf, 0x9);
		uniphier_pphy_test_io(regs, i, 13, 0x60, 0x40);
		uniphier_pphy_test_io(regs, i, 27, 0x7, 0x7);
		uniphier_pphy_test_io(regs, i, 28, 0x3, 0x1);
	}

	/* negate and release reset by XHCI LINK RESET */
	tmp = readl(regs + UNIPHIER_PXS2_DWC3_RESET);
	tmp &= ~UNIPHIER_PXS2_DWC3_RESET_XLINK;
	writel(tmp, regs + UNIPHIER_PXS2_DWC3_RESET);
	udelay(1000);
	tmp |= UNIPHIER_PXS2_DWC3_RESET_XLINK;
	writel(tmp, regs + UNIPHIER_PXS2_DWC3_RESET);

	return 0;
}

/* for LD20 */
#define EFUSE_BASE		0x5f900000
#define EFUSE_MON27_REG		0x0254
#define EFUSE_MON28_REG		0x0258

static void uniphier_ld20_hs_phy_setup(void __iomem *regs, int hs_instances);

static int uniphier_ld20_dwc3_init(void __iomem *regs)
{
	u32 tmp;
	int hs_instances;
	int ss_instances;
	int num_vbus;
	int i;

	/* get the number of HS/SS port from the HW default value */
	tmp = readl(regs + UNIPHIER_PXS2_DWC3_HOST_CFG);
	hs_instances = (tmp >> UNIPHIER_PXS2_DWC3_HOST_CFG_NUM_U2_SHIFT)
		 & UNIPHIER_PXS2_DWC3_HOST_CFG_NUM_U_MASK;
	ss_instances = (tmp >> UNIPHIER_PXS2_DWC3_HOST_CFG_NUM_U3_SHIFT)
		 & UNIPHIER_PXS2_DWC3_HOST_CFG_NUM_U_MASK;

	/* number of VBUS */
	num_vbus = hs_instances;

	/* control the VBUS */
	for (i = 0; i < num_vbus; i++) {
		/* enable the control and turn-on the VBUS */
		tmp = readl(regs + UNIPHIER_PXS2_DWC3_VBUS_CONTROL(i));
		tmp |= UNIPHIER_PXS2_DWC3_VBUS_CONTROL_VBUS_BIT_EN |
			 UNIPHIER_PXS2_DWC3_VBUS_CONTROL_VBUS_BIT_ONOFF;
		writel(tmp, regs + UNIPHIER_PXS2_DWC3_VBUS_CONTROL(i));
	}

	/* set up SS-PHY */
	for (i = 0; i < ss_instances; i++) {
		/* parameter number 07, 13, 26 */
		uniphier_pphy_test_io(regs, i,  7, 0xff, 0x06);
		uniphier_pphy_test_io(regs, i, 13, 0xff, 0xcc);
		uniphier_pphy_test_io(regs, i, 26, 0xff, 0x50);
	}

	/* set up HS-PHY */
	uniphier_ld20_hs_phy_setup(regs, hs_instances);

	/* negate and release reset by XHCI LINK RESET */
	tmp = readl(regs + UNIPHIER_PXS2_DWC3_RESET);
	tmp &= ~UNIPHIER_PXS2_DWC3_RESET_XLINK;
	writel(tmp, regs + UNIPHIER_PXS2_DWC3_RESET);
	udelay(1000);
	tmp |= UNIPHIER_PXS2_DWC3_RESET_XLINK;
	writel(tmp, regs + UNIPHIER_PXS2_DWC3_RESET);

	return 0;
}

static void uniphier_hs_phy_int_param_set(void __iomem *regs, int port,
					u32 addr, u32 data)
{
	void __iomem *vptr;
	u32 tmp;

	vptr = regs + UNIPHIER_PXS2_DWC3_U2PHY_CFG1(port);
	tmp = readl(vptr);

	/* set the internal parameter number */
	tmp = (tmp & 0x0000ffff) | ((addr & 0xfff) << 16) | (0x1 << 28);
	writel(tmp, vptr);

	tmp = (tmp & 0x0000ffff) | ((addr & 0xfff) << 16);
	writel(tmp, vptr);

	/* set the internal parameter data */
	tmp = (tmp & 0x0000ffff) | ((data & 0xff) << 16) | (0x1 << 29);
	writel(tmp, vptr);

	tmp = (tmp & 0x0000ffff) | ((data & 0xff) << 16);
	writel(tmp, vptr);
}

static void uniphier_ld20_hs_phy_setup(void __iomem *regs, int hs_instances)
{
	int i;
	u32 tmp;

	u32 efuse[2];
	u32 efuse_rterm_trim;
	u32 efuse_rtim_sel_t;
	u32 efuse_hs_i_trim;

	u32 hs_phy_cfgl, hs_phy_cfgh;

	/* read the customized phy parameter from efuse */
	efuse[0] = readl(EFUSE_BASE + EFUSE_MON27_REG);
	efuse[1] = readl(EFUSE_BASE + EFUSE_MON28_REG);

	for (i = 0; i < hs_instances; i++) {
		/* set the default recommended value */
		/* change [27:26] 0x0->0x3 */
		hs_phy_cfgl = 0x9e306680;
		hs_phy_cfgh = 0x00000106;

		/* override by the value from efuse, if exist */
		tmp = efuse[i / 2];
		if (tmp != 0) {
			/* P0/2 efuse27/28 [5:4],[3:0],[19:16] */
			/* P1/3 efuse27/28 [13:12],[11:8],[19:16] */
			efuse_rterm_trim = (tmp >> (4 + (i % 2) * 8)) & 0x3;
			efuse_rtim_sel_t = (tmp >> (0 + (i % 2) * 8)) & 0xf;
			efuse_hs_i_trim  = (tmp >> 16) & 0xf;

			/* clear [31:28][15:12][7:6] and override them */
			/* clear [27:26] */
			hs_phy_cfgl &= 0x03ff0f3f;
			hs_phy_cfgl |= efuse_rterm_trim << 6;
			hs_phy_cfgl |= efuse_rtim_sel_t << 12;
			hs_phy_cfgl |= efuse_hs_i_trim << 28;
		}

		/* write to the registers for the i-th HS instance */
		writel(hs_phy_cfgl, regs + UNIPHIER_PXS2_DWC3_U2PHY_CFG0(i));
		writel(hs_phy_cfgh, regs + UNIPHIER_PXS2_DWC3_U2PHY_CFG1(i));

		/* set the HS swing parameter value */
		tmp = readl(regs + UNIPHIER_PXS2_DWC3_U2PHY_CFG0(i));
		tmp &= ~UNIPHIER_PXS2_DWC3_U2PHY_CFG0_HS_SWING_SEL_MASK;
		tmp |= 0x1 << UNIPHIER_PXS2_DWC3_U2PHY_CFG0_HS_SWING_SEL_SHIFT;
		writel(tmp, regs + UNIPHIER_PXS2_DWC3_U2PHY_CFG0(i));

		/* set the internal parameter value */
		/* parameter number 10 is set to 0x60 */
		uniphier_hs_phy_int_param_set(regs, i, 10, 0x60);
	}
}

static int uniphier_pxs3_dwc3_init(void __iomem *regs)
{
	u32 tmp;
	int hs_instances;
	int num_vbus;
	int i;

	/* get the number of HS/SS port from the HW default value */
	tmp = readl(regs + UNIPHIER_PXS2_DWC3_HOST_CFG);
	hs_instances = (tmp >> UNIPHIER_PXS2_DWC3_HOST_CFG_NUM_U2_SHIFT)
		 & UNIPHIER_PXS2_DWC3_HOST_CFG_NUM_U_MASK;

	/* number of VBUS */
	num_vbus = hs_instances;

	/* control the VBUS */
	for (i = 0; i < num_vbus; i++) {
		/* enable the control and turn-on the VBUS */
		tmp = readl(regs + UNIPHIER_PXS2_DWC3_VBUS_CONTROL(i));
		tmp |= UNIPHIER_PXS2_DWC3_VBUS_CONTROL_VBUS_BIT_EN |
			 UNIPHIER_PXS2_DWC3_VBUS_CONTROL_VBUS_BIT_ONOFF;
		writel(tmp, regs + UNIPHIER_PXS2_DWC3_VBUS_CONTROL(i));
	}

	/* set up SS-PHY */
	/* for future , if need */

	/* set up HS-PHY */
	/* for future , if need */

	/* negate and release reset by XHCI LINK RESET */
	tmp = readl(regs + UNIPHIER_PXS2_DWC3_RESET);
	tmp &= ~UNIPHIER_PXS2_DWC3_RESET_XLINK;
	writel(tmp, regs + UNIPHIER_PXS2_DWC3_RESET);
	udelay(1000);
	tmp |= UNIPHIER_PXS2_DWC3_RESET_XLINK;
	writel(tmp, regs + UNIPHIER_PXS2_DWC3_RESET);

	return 0;
}

static int uniphier_dwc3_probe(struct udevice *dev)
{
	fdt_addr_t base;
	void __iomem *regs;
	int (*init)(void __iomem *regs);
	int ret;

	base = devfdt_get_addr(dev);
	if (base == FDT_ADDR_T_NONE)
		return -EINVAL;

	regs = ioremap(base, SZ_32K);
	if (!regs)
		return -ENOMEM;

	init = (typeof(init))dev_get_driver_data(dev);
	ret = init(regs);
	if (ret)
		dev_err(dev, "failed to init glue layer\n");

	iounmap(regs);

	return ret;
}

static const struct udevice_id uniphier_dwc3_match[] = {
	{
		.compatible = "socionext,uniphier-pro4-dwc3",
		.data = (ulong)uniphier_pro4_dwc3_init,
	},
	{
		.compatible = "socionext,uniphier-pro5-dwc3",
		.data = (ulong)uniphier_pro5_dwc3_init,
	},
	{
		.compatible = "socionext,uniphier-pxs2-dwc3",
		.data = (ulong)uniphier_pxs2_dwc3_init,
	},
	{
		.compatible = "socionext,uniphier-ld20-dwc3",
		.data = (ulong)uniphier_ld20_dwc3_init,
	},
	{
		.compatible = "socionext,uniphier-pxs3-dwc3",
		.data = (ulong)uniphier_pxs3_dwc3_init,
	},
	{ /* sentinel */ }
};

U_BOOT_DRIVER(usb_xhci) = {
	.name = "uniphier-dwc3",
	.id = UCLASS_SIMPLE_BUS,
	.of_match = uniphier_dwc3_match,
	.probe = uniphier_dwc3_probe,
};
