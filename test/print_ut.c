/*
 * Copyright (c) 2012, The Chromium Authors
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#define DEBUG

#include <common.h>
#include <display_options.h>
#include <version.h>

#define FAKE_BUILD_TAG	"jenkins-u-boot-denx_uboot_dm-master-build-aarch64" \
			"and a lot more text to come"

static int do_ut_print(cmd_tbl_t *cmdtp, int flag, int argc,
		       char *const argv[])
{
	char big_str[400];
	int big_str_len;
	char str[10], *s;
	int len;

	printf("%s: Testing print\n", __func__);

	snprintf(str, sizeof(str), "testing");
	BUG_ON(strcmp("testing", str));

	snprintf(str, sizeof(str), "testing but too long");
	BUG_ON(strcmp("testing b", str));

	snprintf(str, 1, "testing none");
	BUG_ON(strcmp("", str));

	*str = 'x';
	snprintf(str, 0, "testing none");
	BUG_ON(*str != 'x');

	sprintf(big_str, "_%ls_", L"foo");
	BUG_ON(strcmp("_foo_", big_str));

	/* Test the banner function */
	s = display_options_get_banner(true, str, sizeof(str));
	BUG_ON(s != str);
	BUG_ON(strcmp("\n\nU-Boo\n\n", s));

	s = display_options_get_banner(true, str, 1);
	BUG_ON(s != str);
	BUG_ON(strcmp("", s));

	s = display_options_get_banner(true, str, 2);
	BUG_ON(s != str);
	BUG_ON(strcmp("\n", s));

	s = display_options_get_banner(false, str, sizeof(str));
	BUG_ON(s != str);
	BUG_ON(strcmp("U-Boot \n\n", s));

	/* Give it enough space for some of the version */
	big_str_len = strlen(version_string) - 5;
	s = display_options_get_banner_priv(false, FAKE_BUILD_TAG, big_str,
					    big_str_len);
	BUG_ON(s != big_str);
	BUG_ON(strncmp(version_string, s, big_str_len - 3));
	BUG_ON(strcmp("\n\n", s + big_str_len - 3));

	/* Give it enough space for the version and some of the build tag */
	big_str_len = strlen(version_string) + 9 + 20;
	s = display_options_get_banner_priv(false, FAKE_BUILD_TAG, big_str,
					    big_str_len);
	BUG_ON(s != big_str);
	len = strlen(version_string);
	BUG_ON(strncmp(version_string, s, len));
	BUG_ON(strncmp(", Build: ", s + len, 9));
	BUG_ON(strncmp(FAKE_BUILD_TAG, s + 9 + len, 12));
	BUG_ON(strcmp("\n\n", s + big_str_len - 3));

	printf("%s: Everything went swimmingly\n", __func__);
	return 0;
}

U_BOOT_CMD(
	ut_print,	1,	1,	do_ut_print,
	"Very basic test of printf(), etc.",
	""
);
