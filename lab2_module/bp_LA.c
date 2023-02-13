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
MODULE_DESCRIPTION("Module BP");

// Structure to represent the layout of the memory-mapped I/O for the Raspberry Pi's GPIO
struct gpio_s
{
    uint32_t gpfsel[7]; // Function select registers
    uint32_t gpset[3]; // Pin output set registers
    uint32_t gpclr[3]; // Pin output clear registers
    uint32_t gplev[3]; // Pin level registers
    uint32_t gpeds[3]; // Pin event detect status registers
    uint32_t gpren[3]; // Pin rising edge detect enable registers
    uint32_t gpfen[3]; // Pin falling edge detect enable registers
    uint32_t gphen[3]; // Pin high detect enable registers
    uint32_t gplen[3]; // Pin low detect enable registers
    uint32_t gparen[3]; // Pin async rising edge detect registers
    uint32_t gpafen[3]; // Pin async falling edge detect registers
    uint32_t gppud[1]; // Pin pull-up/down enable register
    uint32_t gppudclk[3]; // Pin pull-up/down clock register
    uint32_t test[1]; // Test register
}
// Pointer to the base address of the GPIO I/O
volatile *gpio_regs = (struct gpio_s *)__io_address(GPIO_BASE);

/* gpio_fsel - function to configure the function of a specific GPIO pin */
static void 
gpio_fsel(int pin, int fun)
{
    /* Calculate the register number for the given pin */
    uint32_t reg = pin / 10;

    /* Calculate the bit position for the given pin */
    uint32_t bit = (pin % 10) * 3;

    /* Create a mask for the register */
    uint32_t mask = 0b111 << bit;

    /* Update the register field for the given pin */
    gpio_regs->gpfsel[reg] = (gpio_regs->gpfsel[reg] & ~mask) | ((fun << bit) & mask);
}

/* gpio_read - function to read the value of a specific GPIO pin */
static int 
gpio_read(int pin){

    /* Calculate the register number for the given pin */
    uint32_t reg = pin/32;

    /* Calculate the bit position for the given pin */
    uint32_t bit = pin%32;

    /* Read the value of the given pin and return the result */
    return (gpio_regs->gplev[reg]&(1<<bit)) != 0;
}

/* major - variable to store the major number of the device */
static int major;
// Definition of the open function for the BP driver
static int 
open_bp_LA(struct inode *inode, struct file *file) {

    // Log message indicating that the BP has been initialized
    printk(KERN_DEBUG "BP initialised!\n");
    
    // Setup GPIO of BP to input
    // ---------------------------------------------
    gpio_fsel(GPIO_BP, GPIO_FSEL_INPUT);

    // Log message indicating that the BP can now be read
    printk(KERN_DEBUG "BP can be read!\n");

    // Return 0 to indicate successful completion of the open function
    return 0;
}

// Definition of the read function for the BP driver
static ssize_t 
read_bp_LA(struct file *file, char *buf, size_t count, loff_t *ppos) {
    // Log message indicating that the BP is being read
    printk(KERN_DEBUG "Reading BP!\n");
    // Check the value of the BP
    if (gpio_read(GPIO_BP)) {
        // Log message indicating a high value for the BP
        printk(KERN_DEBUG "bp : 1\n");
        // Store the high value in the buffer
        buf[0] = "1";
    }else if (gpio_read(GPIO_BP)) {
        // Log message indicating a low value for the BP
        printk(KERN_DEBUG "bp : 0\n");
        // Store the low value in the buffer
        buf[0] = "0";
    }
    // Return the count to indicate successful completion of the read function
    return count;
}

// Definition of the write function for the BP driver
static ssize_t 
write_bp_LA(struct file *file, const char *buf, size_t count, loff_t *ppos) {
    // Log message indicating that writing to the BP is not allowed
    printk(KERN_DEBUG "ERROR: Can not write to bp!\n");
    // Return the count to indicate successful completion of the write function
    return count;
}

// Definition of the release function for the BP driver
static int 
release_bp_LA(struct inode *inode, struct file *file) {
    // Log message indicating that the BP has stopped working
    printk(KERN_DEBUG "BP stop work!\n");
    // Return 0 to indicate successful completion of the release function
    return 0;
}

struct file_operations fops_bp =
{
    // Open function is called when a user-space process opens the device file. 
    .open       = open_bp_LA,

    // Read function is called when a user-space process reads the device file. 
    .read       = read_bp_LA,

    // Write function is called when a user-space process writes to the device file. 
    .write      = write_bp_LA,

    // Release function is called when a user-space process closes the device file. 
    .release    = release_bp_LA
};

// This function is called when the module is loaded into the kernel.
static int __init mon_module_init(void)
{
    // Register the character device driver using the register_chrdev function.
    // The first argument specifies the major number of the device file. 
    // A value of 0 means to let the kernel choose a free major number.
    // The second argument specifies the name of the device file, which will be "bp_LA".
    // The third argument is a pointer to the struct file_operations that define the functions 
    // to be called when the device file is opened, read, written, and closed.
    major = register_chrdev(0, "bp_LA", &fops_bp);        // 0 To let linux chose the major number
    printk(KERN_DEBUG "bp_LA connected!\n");
    
}

// This function is called when the module is unloaded from the kernel.
static void __exit mon_module_cleanup(void)
{
    // Unregister the character device driver using the unregister_chrdev function.
    // The first argument specifies the major number of the device file.
    // The second argument specifies the name of the device file.
    unregister_chrdev(major, "bp_LA");                     // Fonction pour dechatger le driver
    printk(KERN_DEBUG "bp_LA deconnected!\n");              // Indique la fin du module
}

// Macro that specifies the functions to be called when the module is loaded and unloaded.
module_init(mon_module_init);
module_exit(mon_module_cleanup);
