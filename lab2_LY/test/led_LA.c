#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

static int major;

static int 
open_led_LA(struct inode *inode, struct file *file) {
    printk(KERN_DEBUG "open()\n");
    return 0;
}

static ssize_t 
read_led_LA(struct file *file, char *buf, size_t count, loff_t *ppos) {
    printk(KERN_DEBUG "read()\n");
    return count;
}

static ssize_t 
write_led_LA(struct file *file, const char *buf, size_t count, loff_t *ppos) {
    printk(KERN_DEBUG "write()\n");
    return count;
}

static int 
release_led_LA(struct inode *inode, struct file *file) {
    printk(KERN_DEBUG "close()\n");
    return 0;
}

struct file_operations fops_led =
{
    .open       = open_led_LA,
    .read       = read_led_LA,
    .write      = write_led_LA,
    .release    = release_led_LA 
};

MODULE_LICENSE("GPL");
MODULE_AUTHOR("li_authier, 2023");
MODULE_DESCRIPTION("Module_led_LA");


static int __init mon_module_init(void)
{
    // enregistrement du driver
    printk(KERN_DEBUG "Start LED0_LA!\n");              // Indique la fin du module
    major = register_chrdev(0, "led_LA", &fops_led);    // 0 To let linux chose the major number
    return 0;
}

static void __exit mon_module_cleanup(void)
{
    unregister_chrdev(major, "led_LA");                 // Fonction pour dechatger le driver
    printk(KERN_DEBUG "LED0_LA finish!\n");             // Indique la fin du module
}

module_init(mon_module_init);
module_exit(mon_module_cleanup);
