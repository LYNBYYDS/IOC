#include <linux/module.h>
#include <linux/init.h>
#include <asm/io.h>
#include <mach/platform.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("li_authier, 2023");
MODULE_DESCRIPTION("Module");

static const int LED0 = 4;

struct gpio_s
{
    uint32_t gpfsel[7];
    uint32_t gpset[3];
    uint32_t gpclr[3];
    uint32_t gplev[3];
    uint32_t gpeds[3];
    uint32_t gpren[3];
    uint32_t gpfen[3];
    uint32_t gphen[3];
    uint32_t gplen[3];
    uint32_t gparen[3];
    uint32_t gpafen[3];
    uint32_t gppud[1];
    uint32_t gppudclk[3];
    uint32_t test[1];
}
volatile *gpio_regs = (struct gpio_s *)__io_address(GPIO_BASE);

/* Focntion de controle des GPIO*/
static void gpio_fsel(int pin, int fun)
{
    uint32_t reg = pin / 10;
    uint32_t bit = (pin % 10) * 3;
    uint32_t mask = 0b111 << bit;
    gpio_regs->gpfsel[reg] = (gpio_regs->gpfsel[reg] & ~mask) | ((fun << bit) & mask);
}

static void gpio_write(int pin, bool val)
{
    if (val)
        gpio_regs->gpset[pin / 32] = (1 << (pin % 32));
    else
        gpio_regs->gpclr[pin / 32] = (1 << (pin % 32));
}

/* Fonction du driver*/
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



#define NBMAX_LED 32
static int leds[NBMAX_LED];
static int nbled;
module_param_array(leds, int, &nbled, 0);
MODULE_PARM_DESC(LEDS, "tableau des numéros de port LED");

static int major;

static int __init mon_module_init(void)
{
    printk(KERN_DEBUG "driver_led_bp_LA LED0_LA!\n");

    // enregistrement du driver
    major = register_chrdev(0, "driver_led_bp_LA", &fops_led); // 0 To let linux chose the major number

    int i;
    for (i=0; i < nbled; i++)
        printk(KERN_DEBUG "LED %d = %d\n", i, leds[i]);
    return 0;
}

static void __exit mon_module_cleanup(void)
{
    unregister_chrdev(major, "driver_led_bp_LA");            // Fonction pour dechatger le driver
    printk(KERN_DEBUG "Finish driver_led_bp_LA!\n");  // Indique la fin du module
}

module_init(mon_module_init);
module_exit(mon_module_cleanup);
