/*
 * Kernel module "hellomoto" for testing purposes.
 * Written by EXL, 27-Sep-2016.
 * Edited by EXL, 27-Oct-2021.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>

#include <asm/uaccess.h>

#define PROC_ENTRY_FILENAME "hellomoto"
#define KERNEL_MESSAGE_BUFFER_LENGTH 64
#define USER_MESSAGE_BUFFER_LENGTH KERNEL_MESSAGE_BUFFER_LENGTH + 16

#ifdef MOTOMAGX
#define REMOVE_PROC_ENTRY remove_proc_entry(PROC_ENTRY_FILENAME, &proc_root)
#else
#define REMOVE_PROC_ENTRY remove_proc_entry(PROC_ENTRY_FILENAME, NULL)
#endif

static char g_str_message_buffer[KERNEL_MESSAGE_BUFFER_LENGTH] = { '\0' };
static struct proc_dir_entry *g_ptr_proc_file = NULL;

static ssize_t module_output(struct file *filp, char *buffer, size_t length, loff_t *offset) {
	static int finished = 0;
	int i;
	char message[USER_MESSAGE_BUFFER_LENGTH];
	if (finished) {
		finished = 0;
		return 0;
	}
	snprintf(message, USER_MESSAGE_BUFFER_LENGTH, "Last input: %s\n", g_str_message_buffer);
	for (i = 0; i < length && message[i]; ++i)
		put_user(message[i], buffer + i);
	finished = 1;
	return i;
}

static ssize_t module_input(struct file *filp, const char *buff, size_t len, loff_t *off) {
	int i;
	unsigned short keycode;

	for (i = 0; i < KERNEL_MESSAGE_BUFFER_LENGTH - 1 && i < len; ++i)
		get_user(g_str_message_buffer[i], buff + i);
	g_str_message_buffer[i] = '\0';

	keycode = simple_strtoul(g_str_message_buffer, NULL, 0);
	printk(KERN_ALERT "hellomoto: Keycode: 0x%04X, Dec: %hu!\n", keycode, keycode);

#ifdef MOTOMAGX
	mxc_kpp_handle_mode(keycode);
#endif

	return i;
}

static int module_open(struct inode *inode, struct file *file) {
	try_module_get(THIS_MODULE);
	return 0;
}

static int module_close(struct inode *inode, struct file *file) {
	module_put(THIS_MODULE);
	return 0;
}

#ifdef MOTOMAGX
static struct file_operations g_struct_file_ops = {
	.read = module_output,
	.write = module_input,
	.open = module_open,
	.release = module_close,
};

static int module_permission(struct inode *inode, int op, struct nameidata *foo) {
	/* 2: write, 4: read */
	if (op == 4 || op == 2)
		return 0;
	return -EACCES;
}

static struct inode_operations g_struct_inode_ops = {
	.permission = module_permission,
};
#else
static struct proc_ops g_struct_proc_ops = {
	.proc_read = module_output,
	.proc_write = module_input,
	.proc_open = module_open,
	.proc_release = module_close,
};
#endif

static int hellomoto_init(void) {
	printk(KERN_ALERT "hellomoto: Hello, MotoMAGX modders!\n");
#ifdef MOTOMAGX
	g_ptr_proc_file = create_proc_entry(PROC_ENTRY_FILENAME, 0666, NULL);
	g_ptr_proc_file->owner = THIS_MODULE;
	g_ptr_proc_file->proc_iops = &g_struct_inode_ops;
	g_ptr_proc_file->proc_fops = &g_struct_file_ops;
	g_ptr_proc_file->mode = S_IFREG | S_IRUGO | S_IWUSR;
	g_ptr_proc_file->uid = 0;
	g_ptr_proc_file->gid = 0;
	g_ptr_proc_file->size = 80;
#else
	g_ptr_proc_file = proc_create(PROC_ENTRY_FILENAME, 0666, NULL, &g_struct_proc_ops);
#endif
	if (g_ptr_proc_file == NULL) {
		REMOVE_PROC_ENTRY;
		printk(KERN_ALERT "hellomoto: Could not initialize \"/proc/hellomoto\", sorry!\n");
		return -ENOMEM;
	}
	return 0;
}

static void hellomoto_exit(void) {
	REMOVE_PROC_ENTRY;
	printk(KERN_ALERT "hellomoto: Goodbye, MotoMAGX modders!\n");
}

module_init(hellomoto_init);
module_exit(hellomoto_exit);

MODULE_AUTHOR("EXL");
MODULE_DESCRIPTION("Kernel module for testing purposes");
MODULE_LICENSE("GPL");
