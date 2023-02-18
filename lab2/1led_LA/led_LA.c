#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

#define NBMAX_LED 32

static int leds[NBMAX_LED];
static int nbled;

static int major;

// Forward declarations
static int open_led_LA(struct inode *inode, struct file *file);
static ssize_t read_led_LA(struct file *file, char *buf, size_t count, loff_t *ppos);
static ssize_t write_led_LA(struct file *file, const char *buf, size_t count, loff_t *ppos);
static int release_led_LA(struct inode *inode, struct file *file);

// File operations struct
static struct file_operations fops_led0_LA = {
    .open       = open_led_LA,
    .read       = read_led_LA,
    .write      = write_led_LA,
    .release    = release_led_LA
};

// Module parameters
module_param_array(leds, int, &nbled, 0);
MODULE_PARM_DESC(LEDS, "tableau des numeros de port LED");

// Module initialization
static int __init mon_module_init(void)
{
    int i;
    major = register_chrdev(0, "led_LA", &fops_led0_LA);
    printk(KERN_DEBUG "Start LED0_LA!\n");
    for (i = 0; i < nbled; i++) {
        printk(KERN_DEBUG "LED %d = %d\n", i, leds[i]);
    }
    return 0;
}

// Module cleanup
static void __exit mon_module_cleanup(void)
{
    unregister_chrdev(major, "led_LA");
    printk(KERN_DEBUG "Finish of LED0_LA!\n");
}

// File operations implementation
static int open_led_LA(struct inode *inode, struct file *file) {
    printk(KERN_DEBUG "open()\n");
    return 0;
}

static ssize_t read_led_LA(struct file *file, char *buf, size_t count, loff_t *ppos) {
    printk(KERN_DEBUG "read()\n");
    return count;
}

static ssize_t write_led_LA(struct file *file, const char *buf, size_t count, loff_t *ppos) {
    printk(KERN_DEBUG "write()\n");
    return count;
}

static int release_led_LA(struct inode *inode, struct file *file) {
    printk(KERN_DEBUG "close()\n");
    return 0;
}

// Module metadata
MODULE_LICENSE("GPL");
MODULE_AUTHOR("li_authier, 2023");
MODULE_DESCRIPTION("led_LA");

module_init(mon_module_init);
module_exit(mon_module_cleanup);
