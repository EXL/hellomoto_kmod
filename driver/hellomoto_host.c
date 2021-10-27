/*
 * Kernel module "hellomoto" for testing purposes.
 * Written by EXL, 27-Sep-2016.
 * Edited by EXL, 27-Oct-2021.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>

#include <asm/uaccess.h>

#define PROC_ENTRY_FILENAME "hellomoto"
#define KERNEL_MESSAGE_BUFFER_LENGTH 64
#define USER_MESSAGE_BUFFER_LENGTH KERNEL_MESSAGE_BUFFER_LENGTH + 16

static char g_str_message_buffer[KERNEL_MESSAGE_BUFFER_LENGTH];
static struct proc_dir_entry *g_ptr_proc_file = NULL;

static ssize_t module_output(struct file *filp, char *buffer, size_t length, loff_t *offset) {
	static int finished = 0;
	int i;
	char message[USER_MESSAGE_BUFFER_LENGTH];
	if (finished) {
		finished = 0;
		return 0;
	}
	snprintf(message, USER_MESSAGE_BUFFER_LENGTH, "Last input: %s", g_str_message_buffer);
	for (i = 0; i < length && message[i]; ++i)
		put_user(message[i], buffer + i);
	finished = 1;
	return i;
}

static ssize_t module_input(struct file *filp, const char *buff, size_t len, loff_t *off) {
	int i;
	for (i = 0; i < KERNEL_MESSAGE_BUFFER_LENGTH - 1 && i < len; ++i)
		get_user(g_str_message_buffer[i], buff + i);
	g_str_message_buffer[i] = '\0';
	return i;
}

static int module_permission(struct inode *inode, int op, struct nameidata *foo) {
	/* 2: write, 4: read */
	if (op == 4 || op == 2)
		return 0;
	return -EACCES;
}

static int module_open(struct inode *inode, struct file *file) {
	try_module_get(THIS_MODULE);
	return 0;
}

static int module_close(struct inode *inode, struct file *file) {
	module_put(THIS_MODULE);
	return 0;
}

static struct inode_operations g_struct_inode_ops = {
	.permission = module_permission,
};

static struct file_operations g_struct_file_ops = {
	.read = module_output,
	.write = module_input,
	.open = module_open,
	.release = module_close,
};

int init_module(void) {
	printk(KERN_ALERT "hellomoto: Hello, MotoMAGX modders!\n");
#if 0
	g_ptr_proc_file = create_proc_entry(PROC_ENTRY_FILENAME, 0666, NULL);
	g_ptr_proc_file->owner = THIS_MODULE;
	g_ptr_proc_file->proc_iops = &g_struct_inode_ops;
	g_ptr_proc_file->proc_fops = &g_struct_file_ops;
	g_ptr_proc_file->mode = S_IFREG | S_IRUGO | S_IWUSR;
	g_ptr_proc_file->uid = 0;
	g_ptr_proc_file->gid = 0;
	g_ptr_proc_file->size = 80;
#endif
	g_ptr_proc_file proc_create(PROC_ENTRY_FILENAME, 0666, NULL, &g_struct_file_ops);
	if (g_ptr_proc_file == NULL) {
		remove_proc_entry(PROC_ENTRY_FILENAME, &proc_root);
		printk(KERN_ALERT "hellomoto: Could not initialize \"/proc/hellomoto\", sorry!\n");
		return -ENOMEM;
	}
	return 0;
}

void cleanup_module(void) {
	printk(KERN_ALERT "hellomoto: Goodbye, MotoMAGX modders!\n");
}

MODULE_AUTHOR("EXL");
MODULE_DESCRIPTION("Kernel module for testing purposes");
