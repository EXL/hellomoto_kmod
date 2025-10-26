/*
 * Kernel module "hellomoto" for testing purposes.
 * Written by EXL, 27-Sep-2016.
 * Edited by EXL, 28-Oct-2021.
 */

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm-arm/mot-gpio.h>

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/keypad.h>
#include <linux/slab.h>
#include <linux/power_ic_kernel.h>

#if defined(FTR_HELLOMOTO)

#define PROC_MESSAGE_LENGTH 16
#define PROC_ENTRY_FILENAME "hellomoto"

static struct proc_dir_entry *proc_file = NULL;

static int hellomoto_read(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data) {
	if (offset <= 0) {
		unsigned int message_length;
		char message[PROC_MESSAGE_LENGTH] = { '\0' };

		snprintf(message, PROC_MESSAGE_LENGTH, "0x%08X\n", gpio_signal_get_data_check(GPIO_SIGNAL_LENS_COVER));
		message_length = strlen(message);
		memcpy(buffer, message, message_length);

		return message_length;
	}
	return 0;
}

static int hellomoto_write(struct file *filp, const char *buffer, unsigned long count, void *data) {
	char message[PROC_MESSAGE_LENGTH] = { '\0' };
	unsigned short keycode = 0x0000;
	unsigned long buffer_size = count;

	if (buffer_size > PROC_MESSAGE_LENGTH)
		buffer_size = PROC_MESSAGE_LENGTH;

	if (copy_from_user(message, buffer, buffer_size))
		return -EFAULT;

	keycode = simple_strtoul(message, NULL, 10);
	printk(KERN_INFO "hellomoto: Keycode: 0x%04X, Dec: %hu!\n", keycode, keycode);
	generate_key_event(keycode, KEYDOWN);
	generate_key_event(keycode, KEYUP);

	return buffer_size;
}

static int hellomoto_permission(struct inode *inode, int op, struct nameidata *foo) {
	if (op == 4 || op == 2) /* 2: write, 4: read */
		return 0;
	return -EACCES;
}

static struct inode_operations iops = { .permission = hellomoto_permission };

static int hellomoto_init(void) {
	printk(KERN_INFO "hellomoto: Hello, MotoMAGX modders!\n");
	proc_file = create_proc_entry(PROC_ENTRY_FILENAME, 0644, NULL);
	if (!proc_file) {
		remove_proc_entry(PROC_ENTRY_FILENAME, &proc_root);
		printk(KERN_ERR "hellomoto: Could not initialize \"/proc/hellomoto\", sorry!\n");
		return -ENOMEM;
	}
	proc_file->owner = THIS_MODULE;
	proc_file->proc_iops = &iops;
	proc_file->read_proc = hellomoto_read;
	proc_file->write_proc = hellomoto_write;
	return 0;
}

static void hellomoto_exit(void) {
	printk(KERN_INFO "hellomoto: Goodbye, MotoMAGX modders!\n");
	remove_proc_entry(PROC_ENTRY_FILENAME, &proc_root);
}

module_init(hellomoto_init);
module_exit(hellomoto_exit);

#elif defined(FTR_DUMP_IROM)

/*
 * Thanks to the Grok (grok.com) AI.
 */

#define PROC_ENTRY_FILENAME "iroms"
#define DUMP_SIZE 0x4000
#define TOTAL_SIZE (DUMP_SIZE * 2)

static struct proc_dir_entry *proc_file = NULL;
static char *dump_data = NULL;

static ssize_t iroms_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
	size_t len;

	if (*ppos >= TOTAL_SIZE) {
		return 0;
	}

	len = min(count, TOTAL_SIZE - (size_t)*ppos);

	if (copy_to_user(buf, dump_data + *ppos, len)) {
		return -EFAULT;
	}

	*ppos += len;
	return len;
}

static const struct file_operations iroms_fops = {
	.owner = THIS_MODULE,
	.read = iroms_read,
};

static int iroms_init(void) {
	void __iomem *base1;
	void __iomem *base2;

	printk(KERN_INFO "iroms: Initializing IROM dump module.\n");

	dump_data = kmalloc(TOTAL_SIZE, GFP_KERNEL);
	if (!dump_data) {
		printk(KERN_ERR "iroms: Could not allocate memory for dump.\n");
		return -ENOMEM;
	}

	base1 = ioremap(0x00000000, DUMP_SIZE);
	if (!base1) {
		printk(KERN_ERR "iroms: Could not ioremap first region.\n");
		kfree(dump_data);
		return -ENOMEM;
	}

	base2 = ioremap(0x00404000, DUMP_SIZE);
	if (!base2) {
		printk(KERN_ERR "iroms: Could not ioremap second region.\n");
		iounmap(base1);
		kfree(dump_data);
		return -ENOMEM;
	}

	memcpy_fromio(dump_data, base1, DUMP_SIZE);
	memcpy_fromio(dump_data + DUMP_SIZE, base2, DUMP_SIZE);

	iounmap(base1);
	iounmap(base2);

	proc_file = create_proc_entry(PROC_ENTRY_FILENAME, 0444, NULL);
	if (!proc_file) {
		printk(KERN_ERR "iroms: Could not initialize \"/proc/iroms\".\n");
		kfree(dump_data);
		return -ENOMEM;
	}

	proc_file->proc_fops = &iroms_fops;
	proc_file->size = TOTAL_SIZE;

	return 0;
}

static void iroms_exit(void) {
	printk(KERN_INFO "iroms: Exiting IROM dump module.\n");
	remove_proc_entry(PROC_ENTRY_FILENAME, NULL);
	if (dump_data) {
		kfree(dump_data);
	}
}

module_init(iroms_init);
module_exit(iroms_exit);

#else
#error "Unknown kernel module flavor, please check preprocessor flags!"
#endif

MODULE_AUTHOR("EXL");
MODULE_DESCRIPTION("Kernel module for testing purposes");
MODULE_LICENSE("GPL");
