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


static int major;
static struct class *led_class;

/* registers */
/* IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 地址：0x02290000 + 0x14 */
static volatile unsigned int *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3;
/* GPIO5_GDIR 地址：0x020AC004 */
static volatile unsigned int *GPIO5_GDIR;
/* GPIO5_DR 地址：0x020AC000 */
static volatile unsigned int *GPIO5_DR;

static ssize_t led_write(struct file *filp, const char __user *buf,
                size_t count ,loff_t *ppos)
{
    char val = 0;
    int ret;
    /* Read op code from app */
    ret = copy_from_user(&val, buf, 1);
    if (val)
    {
        /* Output low to open led */
        *GPIO5_DR &= ~(1 << 3);
    }
    else
    {
        /* Output high to close led */
        *GPIO5_DR |= (1 << 3);
    }

    return count;
}

static int led_open(struct inode *inode, struct file *filp)
{
    /* Enable GPIO5_IO3 and configure it as gpio */
    *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 &= ~0xf;
    *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 |= 0x5;
    
    /* Configure it as output  */
    *GPIO5_GDIR |= (1 << 3);

    return 0;
}

static struct file_operations led_fops = {
    .owner = THIS_MODULE,
    .open = led_open,
    .write = led_write,
};


static int __init led_init(void)
{
    printk("%s %d\n", __FUNCTION__, __LINE__);
    /* Register character device */
    major = register_chrdev(0, "100ask_led", &led_fops);
    
    /* ioremap */
    IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 = ioremap(0x02290000 + 0x14, 4);
    GPIO5_GDIR = ioremap(0x020AC004, 4);
    GPIO5_DR = ioremap(0x020AC000, 4);

    led_class = class_create(THIS_MODULE, "myled");
    /* Create device /dev/myled */
    device_create(led_class, NULL, MKDEV(major, 0), NULL, "myled");

    return 0;
}

static void __exit led_exit(void)
{
    /* iounmap */
    iounmap(IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3);
    iounmap(GPIO5_GDIR);
    iounmap(GPIO5_DR);

    device_destroy(led_class, MKDEV(major, 0));
    class_destroy(led_class);

    unregister_chrdev(major, "100ask_led");
}

module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");

