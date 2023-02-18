#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("li_authier, 2023");
MODULE_DESCRIPTION("MODULE_1para");

static int btn;
module_param(btn, int, 0);
MODULE_PARM_DESC(btn, "numéro du port du bouton");

static int __init mon_module_init(void)
{
    printk(KERN_DEBUG "Hello World, this is cart 23 controled by li_authier !\n");
    printk(KERN_DEBUG "btn=%d !\n", btn);
    return 0;
}

static void __exit mon_module_cleanup(void)
{
   printk(KERN_DEBUG "Goodbye World, this is cart 23 controled by li_authier !\n");
}

module_init(mon_module_init);
module_exit(mon_module_cleanup);
