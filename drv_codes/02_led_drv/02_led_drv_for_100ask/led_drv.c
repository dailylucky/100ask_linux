#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <asm/io.h>

#include "led_opr.h"

#define LED_NUM 2

static int major;
static struct class *led_class;
static struct led_operations *p_led_opr;


static int led_drv_open(struct inode *inode, struct file *filp)
{
    int minor = iminor(inode);

    printk("%s %d\n", __FUNCTION__, __LINE__);
    /* Initialize led based on device minor number */
    p_led_opr->init(minor);

    return 0;
}

static ssize_t led_drv_read(struct file *file, char __user *buf, size_t size, loff_t *offset)
{
    printk("%s %d\n", __FUNCTION__, __LINE__);
    return 0;
}

static ssize_t led_drv_write(struct file *filp, const char __user *buf,
                size_t count ,loff_t *ppos)
{
    int ret;
    char status = 0;
    struct inode *inode = file_inode(filp);
    int minor = iminor(inode);

    printk("%s %d\n", __FUNCTION__, __LINE__);
    /* Read op code from app */
    ret = copy_from_user(&status, buf, max((size_t)1, count));

    /* Control led based on minor number ans status */
    p_led_opr->ctl(minor, status);

    return count;
}

static int led_drv_close(struct inode *inode, struct file *filp)
{
    printk("%s %d\n", __FUNCTION__, __LINE__);
    return 0;
}

static struct file_operations led_fops = {
    .owner = THIS_MODULE,
    .open = led_drv_open,
    .read = led_drv_read,
    .write = led_drv_write,
    .release = led_drv_close,
};


static int __init led_init(void)
{
    int i = 0;
    
    printk("%s %d\n", __FUNCTION__, __LINE__);
    /* Register character device */
    major = register_chrdev(0, "100ask_led", &led_fops);
    
    p_led_opr = get_board_led_opr();
    
    led_class = class_create(THIS_MODULE, "myled");
    for (i = 0; i < p_led_opr->num; ++i)
    {
        device_create(led_class, NULL, MKDEV(major, i), NULL, "100ask_led%d", i);
    }
    
    
    
    return 0;
}

static void __exit led_exit(void)
{
    int i = 0;

    printk("%s %d\n", __FUNCTION__, __LINE__);
    for (i = 0; i < LED_NUM; ++i)
    {
        device_destroy(led_class, MKDEV(major, i));
    }
    class_destroy(led_class);

    unregister_chrdev(major, "100ask_led");

    /* Clear resource */
    p_led_opr->exit();
}

module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");

