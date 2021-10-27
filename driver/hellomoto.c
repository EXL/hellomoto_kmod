/*
 * Kernel module "hellomoto" for testing purposes.
 * Written by EXL, 27-Sep-2016.
 * Edited by EXL, 27-Oct-2021.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

static int __init hello_moto_init(void) {
	printk("hellomoto: Hello, MotoMAGX modders!\n");
	return 0;
}

static void __exit hello_moto_stop(void) {
	printk("hellomoto: Goodbye, MotoMAGX modders!\n");
	return;
}

module_init(hello_moto_init);
module_exit(hello_moto_stop);

MODULE_AUTHOR("EXL");

#ifdef	MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif
