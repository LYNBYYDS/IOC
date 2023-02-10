#include <linux/module.h>
#include <linux/init.h>
#include <asm/io.h>
#include <mach/platform.h>
#include <unistd.h>

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
MODULE_DESCRIPTION("Module BP");

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
gpio_fsel(uint32_t pin, uint32_t fun)
{
    uint32_t reg = pin / 10;
    uint32_t bit = (pin % 10) * 3;
    uint32_t mask = 0b111 << bit;
    gpio_regs_virt->gpfsel[reg] = (gpio_regs_virt->gpfsel[reg] & ~mask) | ((fun << bit) & mask);
}

static void 
gpio_write (uint32_t pin, uint32_t val)
{
    uint32_t reg = pin / 32;
    uint32_t bit = pin % 32;
    if (val == 1) 
        gpio_regs_virt->gpset[reg] = (1 << bit);
    else
        gpio_regs_virt->gpclr[reg] = (1 << bit);
}

static uint32_t 
gpio_read(uint32_t pin){

    uint32_t reg = pin/32;
    uint32_t bit = pin%32;
    return (gpio_regs_virt->gplev[reg]&(1<<bit)) != 0;
}

static int major;

static int 
open_driver_led_bp_LA(struct inode *inode, struct file *file) {
    printk(KERN_DEBUG "BP initialised()\n");
    
    // Setup GPIO of LED0 to output
    // ---------------------------------------------
    gpio_fsel(GPIO_LED0, GPIO_FSEL_OUTPUT);
    gpio_fsel(GPIO_LED1, GPIO_FSEL_OUTPUT);
    
    
    // Setup GPIO of BP to input
    // ---------------------------------------------
    gpio_fsel(GPIO_BP, GPIO_FSEL_INPUT);

    return 0;
}

static uint32_t 
read_driver_led_bp_LA(struct file *file, char *buf, size_t count, loff_t *ppos) {
    printk(KERN_DEBUG "Read BP\n");
    return gpio_read(GPIO_BP);
}

static ssize_t 
write_driver_led_bp_LA(struct file *file, const char *buf, size_t count, loff_t *ppos) {
    printk(KERN_DEBUG "ERROR: Read BP\n");
    return count;
}

static int 
release_driver_led_bp_LA(struct inode *inode, struct file *file) {
    printk(KERN_DEBUG "BP stop work\n");
    return 0;
}

struct file_operations fops_BP =
{
    .open       = open_BP,
    .read       = read_BP,
    .write      = write_BP,
    .release    = release_BP
};



#define NBMAX_LED 32
static int leds[NBMAX_LED];
static int nbled;
module_param_array(leds, int, &nbled, 0);
MODULE_PARM_DESC(LEDS, "tableau des numéros de port LED");

static int __init mon_module_init(void)
{
    // enregistrement du driver
    major = register_chrdev(0, "led_LA", &fops_led); // 0 To let linux chose the major number

    int i;
    printk(KERN_DEBUG "Start LED0_LA!\n");
    for (i=0; i < nbled; i++)
       printk(KERN_DEBUG "LED %d = %d\n", i, leds[i]);
    return 0;
}

static void __exit mon_module_cleanup(void)
{
    unregister_chrdev(major, "led_LA");            // Fonction pour dechatger le driver
    printk(KERN_DEBUG "Finish of LED0_LA!\n");  // Indique la fin du module
}

module_init(mon_module_init);
module_exit(mon_module_cleanup);
