#ifndef _DM_FDT_H
#define _DM_FDT_H

#define FDT_ROOT_NODE_ADDR_CELLS_DEFAULT 1
#define FDT_ROOT_NODE_SIZE_CELLS_DEFAULT 1

struct udevice;

int fdt_n_addr_cells(struct udevice *dev);
int fdt_n_size_cells(struct udevice *dev);


#endif /* _DM_DT_H */
