// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2021 Dakkshesh <dakkshesh5@gmail.com>.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kprofiles.h>

unsigned int mode = 0;
module_param(mode, uint, 0664);


unsigned int active_mode(void) {
	if (mode == 1) {
		return 1;
	}

	if (mode == 2) {
		return 2;
	}

	if (mode == 3) {
		return 3;
	}

	else {
		pr_info("Invalid value passed, falling back to level 0\n");
		return 0;
	}
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dakkshesh");
MODULE_DESCRIPTION("KernelSpace Profiles");
MODULE_VERSION("1.0.0"); 
