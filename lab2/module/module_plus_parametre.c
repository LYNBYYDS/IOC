#include <linux/module.h>
#include <linux/init.h>

#define NBMAX_LED 32

MODULE_LICENSE("GPL");
MODULE_AUTHOR("li_authier, 2023");
MODULE_DESCRIPTION("Module");

static int leds[NBMAX_LED];
static int nbled;
module_param_array(leds, int, &nbled, 0);
MODULE_PARM_DESC(LEDS, "tableau des numéros de port LED");

static int __init mon_module_init(void)
{
    int i;
    printk(KERN_DEBUG "Hello World, this is cart 23 controled by li_authier !\n");
    for (i=0; i < nbled; i++)
       printk(KERN_DEBUG "LED %d = %d\n", i, leds[i]);
}

static void __exit mon_module_cleanup(void)
{
   printk(KERN_DEBUG "Goodbye World, this is cart 23 controled by li_authier !\n");
}

module_init(mon_module_init);
module_exit(mon_module_cleanup);
