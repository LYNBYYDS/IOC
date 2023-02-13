#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

#include <asm/io.h>
#include <mach/platform.h>

#define GPIO_BASE 0x20200000

#define BCM2835_PERIPH_BASE     0x20000000
#define BCM2835_GPIO_BASE       ( BCM2835_PERIPH_BASE + 0x200000 )

#define GPIO_LED0   4
#define GPIO_LED1   17
#define GPIO_BP     18

#define GPIO_FSEL_INPUT  0
#define GPIO_FSEL_OUTPUT 1

MODULE_LICENSE("GPL");
MODULE_AUTHOR("li_authier, 2023");
MODULE_DESCRIPTION("Module led0");

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

static void 
gpio_fsel(int pin, int fun)
{
    uint32_t reg = pin / 10;
    uint32_t bit = (pin % 10) * 3;
    uint32_t mask = 0b111 << bit;
    gpio_regs->gpfsel[reg] = (gpio_regs->gpfsel[reg] & ~mask) | ((fun << bit) & mask);
}

static void 
gpio_write(int pin, bool val)
{
    if (val)
        gpio_regs->gpset[pin / 32] = (1 << (pin % 32));
    else
        gpio_regs->gpclr[pin / 32] = (1 << (pin % 32));
}

static int major;

static int 
open_led0_LA(struct inode *inode, struct file *file) {

    printk(KERN_DEBUG "LED0 initialised!\n");
    
    // Setup GPIO of LED0 to output
    // ---------------------------------------------
    gpio_fsel(GPIO_LED0, GPIO_FSEL_OUTPUT);

    printk(KERN_DEBUG "LED0 can be write on!\n");

    return 0;
}

static ssize_t 
read_led0_LA(struct file *file, char *buf, size_t count, loff_t *ppos) {
    printk(KERN_DEBUG "ERROR: Can not read on led0!\n");
    return count;
}

static ssize_t 
write_led0_LA(struct file *file, const char *buf, size_t count, loff_t *ppos) {
    printk(KERN_DEBUG "Writing on led0! Value input : %d\n", buf[0]);
    if (buf[0] == '0') {
        printk(KERN_DEBUG "Light off!\n");
        gpio_write(GPIO_LED0, 0);
    }
    else if (buf[0] == '1') {
        printk(KERN_DEBUG "Light on!\n");
        gpio_write(GPIO_LED0, 1);
    }
    return count;
}

static int 
release_led0_LA(struct inode *inode, struct file *file) {
    printk(KERN_DEBUG "LED stop work!\n");
    return 0;
}

struct file_operations fops_led0_LA =
{
    .open       = open_led0_LA,
    .read       = read_led0_LA,
    .write      = write_led0_LA,
    .release    = release_led0_LA
};

static int __init mon_module_init(void)
{
    // enregistrement du driver
    major = register_chrdev(0, "led0_LA", &fops_led0_LA);        // 0 To let linux chose the major number
    printk(KERN_DEBUG "led0_LA connected!\n");
    
}

static void __exit mon_module_cleanup(void)
{
    unregister_chrdev(major, "led0_LA");                     // Fonction pour dechatger le driver
    printk(KERN_DEBUG "led0_LA deconnected!\n");              // Indique la fin du module
}


module_init(mon_module_init);
module_exit(mon_module_cleanup);
