/*
 * SPDX-License-Identifier:	GPL-2.0+
 *
 * These functions are similar to the ones in drivers/of/base.c of Linux.
 */

#include <common.h>
#include <libfdt.h>
#include <dm/device.h>
#include <dm/fdt.h>

#define FDT_ROOT_NODE_ADDR_CELLS_DEFAULT 1
#define FDT_ROOT_NODE_SIZE_CELLS_DEFAULT 1

DECLARE_GLOBAL_DATA_PTR;

int fdt_n_addr_cells(struct udevice *dev)
{
	const fdt32_t *ac;

	do {
		if (dev->parent)
			dev = dev->parent;
		ac = fdt_getprop(gd->fdt_blob, dev->of_offset, "#address-cells", NULL);
		if (ac)
			return fdt32_to_cpu(*ac);
	} while (dev->parent);
	/* No #address-cells property for the root node */
	return FDT_ROOT_NODE_ADDR_CELLS_DEFAULT;
}

int fdt_n_size_cells(struct udevice *dev)
{
	const fdt32_t *sc;

	do {
		if (dev->parent)
			dev = dev->parent;
		sc = fdt_getprop(gd->fdt_blob, dev->of_offset, "#size-cells", NULL);
		if (sc)
			return fdt32_to_cpu(*sc);
	} while (dev->parent);
	/* No #size-cells property for the root node */
	return FDT_ROOT_NODE_SIZE_CELLS_DEFAULT;
}
