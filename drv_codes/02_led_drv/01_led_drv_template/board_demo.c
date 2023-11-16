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
#include "led_opr.h"

/* Init led, which: which led */
static int board_demo_led_init(int which)
{
    printk("%s %d\n", __FUNCTION__, __LINE__);
    return 0;
}

/* Control led specified by which */
static int board_demo_led_ctl(int which, char status)
{
    printk("%s %d\n", __FUNCTION__, __LINE__);
    return 0;
}

static struct led_operations board_demo_led_opr = {
    .init = board_demo_led_init,
    .ctl = board_demo_led_ctl,
};

struct led_operations *get_board_led_opr(void)
{
    return &board_demo_led_opr;
}

