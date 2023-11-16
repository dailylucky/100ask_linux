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
#include <asm/io.h>

#include "led_opr.h"

static volatile unsigned int *CCM_CCGR1 = NULL;
static volatile unsigned int *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 = NULL;
static volatile unsigned int *GPIO5_GDIR = NULL;
static volatile unsigned int *GPIO5_DR = NULL;

/* Init led, which: which led */
static int board_demo_led_init(int which)
{
    unsigned int val;
    
    printk("%s %d\n", __FUNCTION__, __LINE__);
    if (which == 0)
    {
        if (!CCM_CCGR1)
        {
            CCM_CCGR1 = ioremap(0x20C406C, 4);
            IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 = ioremap(0x2290014, 4);
            GPIO5_GDIR = ioremap(0x020AC000 + 0x4, 4);
            GPIO5_DR = ioremap(0x020AC000 + 0, 4);
        }
        /* Enable GPIO5_IO03 */
        *CCM_CCGR1 |= (0x3 << 30);
        
        /* Configure it as GPIO */
        val = *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3;
        val &= ~(0xf);
        val |= 0x5;
        *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 = val;

        /* Configure it to output mode */
        *GPIO5_GDIR |= (0x1 << 3);
    }
    return 0;
}

/* Control led specified by which */
static int board_demo_led_ctl(int which, char status)
{
    printk("%s %d\n", __FUNCTION__, __LINE__);
    if (which == 0)
    {
        if (status)
        {
            /* Open led */
            *GPIO5_DR &= ~(0x1 << 3);
        }
        else
        {
            *GPIO5_DR |= (0x1 << 3);
        }
    }
    return 0;
}

static void board_demo_led_exit(void)
{
    printk("%s %d\n", __FUNCTION__, __LINE__);
    if (CCM_CCGR1)
    {
        iounmap(CCM_CCGR1);
        iounmap(IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3);
        iounmap(GPIO5_GDIR);
        iounmap(GPIO5_DR);
        CCM_CCGR1 = IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 = GPIO5_GDIR = \
            GPIO5_DR = NULL;
    }
    
    return;
}

static struct led_operations board_demo_led_opr = {
    .num = 1,
    .init = board_demo_led_init,
    .ctl = board_demo_led_ctl,
    .exit = board_demo_led_exit,
};

struct led_operations *get_board_led_opr(void)
{
    return &board_demo_led_opr;
}

