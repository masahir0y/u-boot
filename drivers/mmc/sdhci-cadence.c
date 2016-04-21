/*
 * Copyright (C) 2016 Socionext Inc.
 *   Author: Masahiro Yamada <yamada.masahiro@socionext.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <linux/io.h>
#include <linux/sizes.h>
#include <dm/device.h>
#include <mmc.h>
#include <sdhci.h>

struct cdns_sdhci_plat {
	struct mmc_config cfg;
	struct mmc mmc;
};

static int cdns_sdhci_bind(struct udevice *dev)
{
	struct cdns_sdhci_plat *plat = dev_get_platdata(dev);

	return sdhci_bind(dev, &plat->mmc, &plat->cfg);
}

static int cdns_sdhci_probe(struct udevice *dev)
{
	struct cdns_sdhci_plat *plat = dev_get_platdata(dev);
	struct mmc_uclass_priv *upriv = dev_get_uclass_priv(dev);
	struct sdhci_host *host = dev_get_priv(dev);
	fdt_addr_t base;
	int ret;

	base = dev_get_addr(dev);
	if (base == FDT_ADDR_T_NONE)
		return -EINVAL;

	host->name = dev->name;
	host->ioaddr = devm_ioremap(dev, base, SZ_4K);
	host->version = SDHCI_SPEC_300;
	host->quirks |= SDHCI_QUIRK_WAIT_SEND_CMD;

	ret = sdhci_setup_cfg(&plat->cfg, host, 0, 0);
	if (ret)
		return ret;

	upriv->mmc = &plat->mmc;
	host->mmc = &plat->mmc;
	host->mmc->priv = host;

	return sdhci_probe(dev);
}

static const struct udevice_id cdns_sdhci_match[] = {
	{ .compatible = "cdns,cdns-sdhci" },
	{ /* sentinel */ }
};

U_BOOT_DRIVER(cdns_sdhci) = {
	.name = "cdns-sdhci",
	.id = UCLASS_MMC,
	.of_match = cdns_sdhci_match,
	.bind = cdns_sdhci_bind,
	.probe = cdns_sdhci_probe,
	.priv_auto_alloc_size = sizeof(struct sdhci_host),
	.platdata_auto_alloc_size = sizeof(struct cdns_sdhci_plat),
	.ops = &sdhci_ops,
};

#define SDIO_REG_HRS4	    0x10
// delay regs address
  #define REG_DELAY_HS          0x00
  #define REG_DELAY_DEFAULT     0x01
  #define REG_DELAY_UHSI_SDR12  0x02
  #define REG_DELAY_UHSI_SDR25  0x03
  #define REG_DELAY_UHSI_SDR50  0x04
  #define REG_DELAY_UHSI_DDR50  0x05
  #define REG_DELAY_MMC_LEGACY  0x06
  #define REG_DELAY_MMC_SDR     0x07
  #define REG_DELAY_MMC_DDR     0x08

static void sd4_set_dlyvr(void __iomem *ioaddr,
			  unsigned char addr, unsigned char data)
{
	u32 dlyrv_reg;

	dlyrv_reg = data << 8;
	dlyrv_reg |= addr;

	//set data and address
	writel(dlyrv_reg, ioaddr + SDIO_REG_HRS4);
	dlyrv_reg |= (1 << 24);
	//send write request
	writel(dlyrv_reg, ioaddr + SDIO_REG_HRS4);
	dlyrv_reg &= ~(1 << 24);
	//clear write request
	writel(dlyrv_reg, ioaddr + SDIO_REG_HRS4);
}

static void phy_config(void __iomem *ioaddr)
{
	sd4_set_dlyvr(ioaddr, REG_DELAY_DEFAULT, 0x04);
	sd4_set_dlyvr(ioaddr, REG_DELAY_HS, 0x04);
	sd4_set_dlyvr(ioaddr, REG_DELAY_UHSI_SDR50, 0x06);
	sd4_set_dlyvr(ioaddr, REG_DELAY_UHSI_DDR50, 0x16);
}

static int cdns_sdhci_glue_probe(struct udevice *dev)
{
	fdt_addr_t base;
	void __iomem *ioaddr;

	base = dev_get_addr(dev);
	if (base == FDT_ADDR_T_NONE)
		return -EINVAL;

	ioaddr = devm_ioremap(dev, base, SZ_4K);
	phy_config(ioaddr);

	return 0;
}

static const struct udevice_id cdns_sdhci_glue_match[] = {
	{ .compatible = "cdns,cdns-sdhci-glue" },
	{ /* sentinel */ }
};

U_BOOT_DRIVER(cdns_sdhci_glue) = {
	.name = "cdns-sdhci-glue",
	.id = UCLASS_SIMPLE_BUS,
	.of_match = cdns_sdhci_glue_match,
	.probe = cdns_sdhci_glue_probe,
};
