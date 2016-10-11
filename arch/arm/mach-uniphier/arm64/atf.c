/*
 * Copyright (C) 2016 Socionext Inc.
 *   Author: Masahiro Yamada <yamada.masahiro@socionext.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <linux/string.h>
#include <asm/barriers.h>

#define ARMTF_LOAD_ADDR		0x80000000
#define ARMTF_BASE		0x000c0000
#define ARMTF_SIZE		0x00010000

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

typedef void (*bl31_entry_t)(struct atf_bl31_params *bl31_params,
			     void *plat_params);

void spl_board_prepare_for_boot(void)
{
	bl31_entry_t bl31_entry;
	static struct atf_bl31_params bl31_params;
	static struct atf_image_info bl33_image_info;

	bl31_entry = (bl31_entry_t)ARMTF_LOAD_ADDR;

	memcpy((void *)ARMTF_LOAD_ADDR, (void *)ARMTF_BASE, ARMTF_SIZE);

	printf("Jumping to BL31 %p\n", bl31_entry);

	ISB;

	bl33_image_info.image_base = CONFIG_SYS_TEXT_BASE;
	bl31_params.bl33_image_info = &bl33_image_info;

	bl31_entry(&bl31_params , NULL);
}
