/*
 * Copyright (C) 2016 Socionext Inc.
 *   Author: Masahiro Yamada <yamada.masahiro@socionext.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <spl.h>
#include <linux/string.h>
#include <asm/barriers.h>

#include "../boot-mode/boot-device.h"

#define ARMTF_LOAD_ADDR		0x80000000
#define ARMTF_SIZE		0x00010000

#define ARMTF_NOR_BASE		0x000d0000
#define ARMTF_EMMC_SECTOR	0x500

struct atf_param_header {
	u8 type;		/* type of the structure */
	u8 version;    /* version of this structure */
	u16 size;      /* size of this structure in bytes */
	u32 attr;      /* attributes: unused bits SBZ */
};

struct atf_image_info {
	struct atf_param_header h;
	unsigned long image_base;   /* physical address of base of image */
	u32 image_size;    /* bytes read from image file */
};

struct atf_bl31_params {
	struct atf_param_header h;
	struct atf_image_info *bl31_image_info;
	void *bl32_ep_info;
	struct atf_image_info *bl32_image_info;
	void *bl33_ep_info;
	struct atf_image_info *bl33_image_info;
};

int uniphier_spl_emmc_load_image(struct spl_image_info *spl_image,
				 u32 dev_addr);

typedef void (*bl31_entry_t)(struct atf_bl31_params *bl31_params,
			     void *plat_params);

void spl_board_prepare_for_boot(void)
{
	bl31_entry_t bl31_entry;
	static struct atf_bl31_params bl31_params;
	static struct atf_image_info bl33_image_info;
	struct spl_image_info spl_image;
	int ret;

	spl_image.load_addr = ARMTF_LOAD_ADDR;
	spl_image.size = ARMTF_SIZE;

	switch (spl_boot_device_raw()) {
	case BOOT_DEVICE_NOR:
		memcpy((void *)(uintptr_t)spl_image.load_addr, (void *)ARMTF_NOR_BASE,
		       spl_image.size);
		break;
	case BOOT_DEVICE_MMC1:
		ret = uniphier_spl_emmc_load_image(&spl_image,
						   ARMTF_EMMC_SECTOR);
		if (ret)
			return;
		break;
	default:
		return;
	}

	bl31_entry = (bl31_entry_t)(uintptr_t)spl_image.load_addr;

	bl33_image_info.image_base = CONFIG_SYS_TEXT_BASE;
	bl31_params.bl33_image_info = &bl33_image_info;

	bl31_entry(&bl31_params , NULL);
}
