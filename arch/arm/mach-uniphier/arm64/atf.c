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

void spl_board_prepare_for_boot(void)
{
	typedef void (*bl31_entry_t)(void);
	bl31_entry_t bl31_entry;

	bl31_entry = (bl31_entry_t)ARMTF_LOAD_ADDR;

	memcpy((void *)ARMTF_LOAD_ADDR, (void *)ARMTF_BASE, ARMTF_SIZE);

	printf("Jumping to BL31 %p\n", bl31_entry);

	ISB;

	bl31_entry();
}
