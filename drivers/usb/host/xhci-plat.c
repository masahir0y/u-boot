/*
 * Copyright (C) 2016 Socionext Inc.
 *   Author: Masahiro Yamada <yamada.masahiro@socionext.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

struct xhci_plat_platdata {
	phys_addr_t membase;
};

static int xhci_plat_probe(struct udevice *dev)
{
	struct xhci_plat_platdata *platdata = dev_get_platdata(dev);
	struct xhci_hccr *hccr;
	struct xhci_hcor *hcor;

	hccr = (void *)platdata->membase;
	hcor = (void *)platdata->membase +
				HC_LENGTH(xhci_readl(&hccr->cr_capbase));

	return xhci_register(dev, hccr, hcor);
}

static int xhci_plat_remove(struct udevice *dev)
{
	xhci_deregister(dev);

	return 0;
}

static int xhci_plat_ofdata_to_platdata(struct udevice *dev)
{
	struct xhci_plat_platdata *platdata = dev_get_platdata(dev);
	fdt_addr_t base;

	base = dev_get_addr(dev);
	if (base == FDT_ADDR_T_NONE)
		return -EINVAL;

	platdata->membase = base;

	return 0;
}

static const struct udevice_id xhci_plat_of_match[] = {
	{ .compatible = "generic-xhci" },
	{ .compatible = "xhci-platform" },
	{ /* sentinel */ }
};

U_BOOT_DRIVER(xhci-plat) = {
	.name = "xhci-hcd",
	.id = UCLASS_USB,
	.of_match = xhci_plat_of_match,
	.probe = xhci_plat_probe,
	.remove = xhci_plat_remove,
	.ofdata_to_platdata = xhci_plat_ofdata_to_platdata,
	.priv_auto_alloc_size = sizeof(struct xhci_ctrl),
	.platdata_auto_alloc_size = sizeof(struct xhci_plat_platdata),
	.ops = &xhci_usb_ops,
	.flags	= DM_FLAG_ALLOC_PRIV_DMA,
};
