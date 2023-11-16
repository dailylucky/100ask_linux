#include <linux/module.h>

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/gfp.h>

#define MIN(a, b) (a < b ? a : b)

static int major = 0;
static char kernel_buf[1024];
static struct class *hello_class;

static int hello_drv_open(struct inode * inode, struct file * file)
{
    printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}

static ssize_t hello_drv_read(struct file *file, char __user *buf, size_t size, loff_t *offset)
{
    printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
    copy_to_user(buf, kernel_buf, MIN(1024, size));
    return MIN(1024, size);
}

static ssize_t hello_drv_write(struct file *file, const char __user *buf, size_t size, loff_t *offset)
{
    printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
    copy_from_user(kernel_buf, buf, MIN(1024, size));
    return MIN(1024, size);
}

static int hello_drv_close(struct inode * inode, struct file * file)
{
    printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}

static const struct file_operations hello_drv_fops = {
	.owner		= THIS_MODULE,
	.open		= hello_drv_open,
	.read       = hello_drv_read,
	.write      = hello_drv_write,
	.release	= hello_drv_close,
};

/* Init function */
static int __init hello_init(void)
{
    printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
    /* Register character device */
    major = register_chrdev(0, "hello", &hello_drv_fops);
    hello_class = class_create(THIS_MODULE, "hello_class");
    if (IS_ERR(hello_class))
    {
        printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
        unregister_chrdev(major, "hello");
        return -1;
    }

    device_create(hello_class, NULL, MKDEV(major, 0), NULL, "hello");
    
    return 0;
}

/* Exit function */
static void __exit hello_exit(void)
{
    printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
    device_destroy(hello_class, MKDEV(major, 0));
    class_destroy(hello_class);
    unregister_chrdev(major, "hello");
}


module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");



