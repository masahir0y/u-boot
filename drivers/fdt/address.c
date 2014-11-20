/*
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <dm/device.h>
#include <dm/fdt.h>

struct of_bus {
	const char	*name;
	const char	*addresses;
	int		(*match)(struct device_node *parent);
	void		(*count_cells)(struct device_node *child,
				       int *addrc, int *sizec);
	u64		(*map)(__be32 *addr, const __be32 *range,
				int na, int ns, int pna);
	int		(*translate)(__be32 *addr, u64 offset, int na);
	unsigned int	(*get_flags)(const __be32 *addr);
};

static void fdt_bus_default_count_cells(struct udevice *dev,
					int *addrc, int *sizec) /*52*/
{
	if (addrc)
		*addrc = fdt_n_addr_cells(dev);
	if (sizec)
		*sizec = fdt_n_size_cells(dev);
}

static int fdt_bus_pci_match(struct udevice *bus)/*101*/
{
	/* enable when PCI class is implemented */
	/* return bus->uclass->uc_drv->id == UCLASS_PCI; */
	return bus->uclass->uc_drv->id == -100;
}

static void fdt_bus_pci_count_cells(struct udevice *dev,
				    int *addrc, int *sizec)/*121*/
{
	if (addrc)
		*addrc = 3;
	if (sizec)
		*sizec = 2;
}

static struct fdt_buf fdt_busses[] = { /*407*/
	/* PCI */
	{
		.name = "pci",
		.addresses = "assigned-addresses",
		.match = fdt_bus_pci_match,
	},
	/* Default */
	{
		.name = "default",
		.addresses = "reg",
		.match = NULL,
	},
};

static struct fdt_bus *fdt_match_bus(struct udevice *bus)/*442*/
{
	int i;

	for (i = 0; i < ARRAY_SIZE(fdt_busses); i++)
		if (!fdt_busses[i].match || fdt_busses[i].match(bus))
			return &fdt_busses[i];
	BUG();
	return NULL;
}

const fdt32_t *fdt_get_address(struct udevice *dev, int index, u64 *size)/*613*/
{
	const fdt32_t *prop;
	struct udevice *parent;
	struct fdt_bus *bus;
	int psize, onesize, i, na, ns;

	/* Get parent & match bus type */
	parent = dev->parent;
	if (parent == NULL)
		return NULL;
	bus = fdt_match_bus(parent);
	bus->count_cells(dev, &na, &ns);

	if (!OF_CHECK_ADDR_COUNT(na))
		return NULL;

	/* Get "reg" or "assigned-addresses" property */
	prop = fdt_get_property(dev, bus->addresses, &psize);
	if (prop == NULL)
		return NULL;
	psize /= 4;

	onesize = na + ns;
	for (i = 0; psize >= onesize; psize -= onesize, prop += onesize, i++)
		if (i == index) {
			if (size)
				*size = fdt_read_number(prop + na, ns);
			return prop;
		}
	return NULL;
}


static u64 fdt_translate_address(struct udevice *dev,
				 const fdt32_t *in_addr, const char *rprop)
{
	struct device_node *parent = NULL;
	struct of_bus *bus, *pbus;
	__be32 addr[FDT_MAX_NCELLS];
	int na, ns, pna, pns;
	u64 result = OF_BAD_ADDR;

	pr_debug("OF: ** translation for device %s **\n", dev->name);

 	/* Get parent & match bus type */
	parent = dev->parent;
	if (parent == NULL)
		goto bail;
	bus = fdt_match_bus(parent);

	/* Count address cells & copy address locally */
	fdt_bus_default_count_cells(dev, &na, &ns);
	if (!OF_CHECK_COUNTS(na, ns)) {
		pr_debug("OF: Bad cell count for %s\n", of_node_full_name(dev));
		goto bail;
	}
	memcpy(addr, in_addr, na * 4);

	pr_debug("OF: bus is %s (na=%d, ns=%d) on %s\n",
	    bus->name, na, ns, of_node_full_name(parent));
	of_dump_addr("OF: translating address:", addr, na);

	/* Translate */
	for (;;) {
		/* Switch to parent bus */
		dev = parent;
		parent = of_get_parent(dev);

		/* If root, we have finished */
		if (parent == NULL) {
			pr_debug("OF: reached root node\n");
			result = of_read_number(addr, na);
			break;
		}

		/* Get new parent bus and counts */
		pbus = of_match_bus(parent);
		pbus->count_cells(dev, &pna, &pns);
		if (!OF_CHECK_COUNTS(pna, pns)) {
			printk(KERN_ERR "prom_parse: Bad cell count for %s\n",
			       of_node_full_name(dev));
			break;
		}

		pr_debug("OF: parent bus is %s (na=%d, ns=%d) on %s\n",
		    pbus->name, pna, pns, of_node_full_name(parent));

		/* Apply bus translation */
		if (of_translate_one(dev, bus, pbus, addr, na, ns, pna, rprop))
			break;

		/* Complete the move up one level */
		na = pna;
		ns = pns;
		bus = pbus;

		of_dump_addr("OF: one level translation:", addr, na);
	}
 bail:
	of_node_put(parent);
	of_node_put(dev);

	return result;
}

u64 of_translate_address(struct device_node *dev, const __be32 *in_addr)
{
	return __of_translate_address(dev, in_addr, "ranges");
}
